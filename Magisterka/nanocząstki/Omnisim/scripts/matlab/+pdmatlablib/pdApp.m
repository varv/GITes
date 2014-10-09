classdef pdApp < handle
    % PDAPP by Photon Design v.1.2
	% This class can be used to communicate between Photon Design
    % software and MATLAB.
    % For details please refer to Photon Design documentation.
    
    properties 
        appSock % socket used by Matlab
        currPort % port used by Photon Design application (default 5101)
        cmdList % command list
    end
    
    methods
        function obj = pdApp() % class constructor
            % initialises properties
            obj.appSock = []; % socket
            obj.currPort = 5101; % port on which FIMMWAVE is opened
            obj.cmdList = ''; % list of commands to send to FIMMWAVE
        end
        function ConnectToApp(obj, varargin)
            % connects to the application
			% import modules
            import java.net.Socket;
            import java.io.*;
			% we now need to add the class DataReader to the dynamic java path
			javaclasspath = mfilename('fullpath'); % location of pdApp.m file
			tempidx = strfind(javaclasspath,'\');
			javaclasspath = javaclasspath(1:tempidx(length(tempidx))-1); % location of parent folder
			javaaddpath(javaclasspath); % add parent folder to dynamic java path
			% begin function
            global input_socket input_stream d_input_stream
            CONNECTIONATTEMPTS = 10;
            if isempty(varargin)
                port = int32(obj.currPort);
            elseif isnumeric(varargin{1})
                obj.currPort = varargin{1};
                port = int32(obj.currPort);
            else
                port = int32(obj.currPort);
            end
            if obj.appSock ~= []
                disp('This object is already in use');
            else
                a = 0;
                disp(['Attempting to connect to application on TCP/IP Port No. ' int2str(port)]);
                while a<CONNECTIONATTEMPTS
					try
						input_socket = Socket('localhost', port);
						obj.appSock = input_socket;
                        break
                    catch %ConnectException
                        a = a + 1;
                        disp(['Connection Attempt Number ' int2str(a)]);
					end
					disp(a);
                    if (a==CONNECTIONATTEMPTS)
                        disp('Failed to connect to the application');
                    end
                end
                if isempty(input_socket)
                else
                    try
                        input_stream   = input_socket.getInputStream;
                        d_input_stream = DataInputStream(input_stream);
                    catch
                    end
                end
            end
        end
        function delete(obj) 
            % disconnects from the application, needs to be called to close the socket connection
			if obj.appSock ~= []
                obj.appSock.close();
			end
        end
        function AddCmd(obj,command)
            % adds command to cmdList
            if ischar(command)
                obj.cmdList = [obj.cmdList command ';'];
            else
                disp('Error: command must be a string!');
            end
        end
        function [output] = Exec(obj,command)
            % sends cmdList to application and stores result in output
            import java.net.Socket;
            import java.io.*;
            global input_socket input_stream d_input_stream
            
            if isempty(obj.appSock)
                disp('Connection not initialised');
                output = [];
			else
                pw = PrintWriter(input_socket.getOutputStream,true);
                % prepare command
                AddCmd(obj,command); % interpretes variables, adds semi-column
                commlen = length(obj.cmdList)+1;
                commlenstr = int2str(commlen);
                residual_buffer = blanks(20-length(commlenstr)); % adds spaces to complete the first line of 20 characters
                cmd = [commlenstr, residual_buffer, obj.cmdList]; % protocol expected by Photon Design application
                cmd = concat(java.lang.String(cmd), java.lang.Character(char(0)).toString);
                % send
                try
                    pw.print(cmd);
                    pw.flush;
                catch exception
                    disp(exception);
                end
                % receive
                bytes_available = input_stream.available;				
				while bytes_available == 0 % while loop allows us to wait for messages to arrive
                    bytes_available = input_stream.available;
				end
				%fprintf(1, 'Reading %d bytes\n', bytes_available);
				data_reader = DataReader(d_input_stream);
				message = data_reader.readBuffer(bytes_available);
				message = char(message'); % Data comes out as a column vector
                % there could be more data to come!
                bytes_available = input_stream.available;
				while bytes_available ~= 0 % whilst there is more data to come
					%fprintf(1, 'Reading %d bytes\n', bytes_available);
                    %disp('data transfer...');
					data_reader = DataReader(d_input_stream);
					addmessage = data_reader.readBuffer(bytes_available);
                    addmessage = char(addmessage');
					message = [message addmessage];
                    bytes_available = input_stream.available;
				end
                % analyse message
                retval = strfind(message, 'RETVAL:');
                if isempty(retval) % no output, return nothing
                    output = [];
                else % if there is an output
                    if length(retval) == 1 % one RETVAL:, one command
                        message = message(retval+7:length(message));
                        if isempty(deblank(message))
                            output = [];
                        else
                            output = obj.InterpretString3(message);
                        end
                    else % multiple RETVAL:, each corresponding to a different command;
                        % the message is split into a line of cells via regexp, each cell is then treated with interpretstring3
                        message = message(retval(1)+7:length(message));
                        messageArr = regexp(message,'RETVAL:','split');
                        idx = 1;
                        for i = 1:length(messageArr)
                            if isempty(deblank(messageArr{i}))
                            else
                                output{idx}=obj.InterpretString3(messageArr{i});
                                idx = idx+1;
                            end
                        end
                        if idx == 1
                            output = [];
                        end
                        if idx == 2
                            output = output{1};
                        end
                    end
                end
                obj.cmdList = '';
            end
        end
    end
    
    methods (Static)
        function output = InterpretString3(message)
            import pdMatlabLib.* % needed to call static method "NumorStr"
            
            % test whether the message contains brackets as it could be an array
            currIdx = strfind(message,'[');
			nextIdx = strfind(message,']');
			if isempty(currIdx)
                output=pdApp.NumorStr(message); % no [ brackets: not an array
			elseif isempty(nextIdx)
                output=pdApp.NumorStr(message); % no ] brackets: not an array
            else % both [ and ] brackets; could be a list, a 1D array or a 2D array
                arrStr = regexp(message, '\n', 'split'); % separates the lines of the message
                
                % We will now test to see whether this is a list, a 1D array or a 2D array.
                % list format MUST BE:
                % <array-identifier>[integer]
                % 1D array format MUST BE:
                % <array-identifier>[integer]    value
                % 2D array format MUST BE:
                % <array-identifier>[integer][integer]     value
                                
                % test to see whether we have number between the brackets
                currIdx = strfind(arrStr{1},'['); % first '['
                nextIdx = strfind(arrStr{1},']'); % first ']'
                testStr = arrStr{1};
                idx1Start = testStr(currIdx(1)+1:nextIdx(1)-1); % contained between the brackets
                if isempty(str2num(idx1Start)) 
                    output=pdApp.NumorStr(message); % bracket content is not number: not an array
                else % there is a number between the brackets: either a 1D array, a 2D array or a 1D list (no values)
					% remove blanks
                    if length(testStr)>nextIdx(1)+1 % check that ']' is not the last character
                        remain = testStr(nextIdx(1)+1:length(testStr)); % rest of the first line
                    else
                        remain = '';
                    end
                    remain_trimmed = deblank(remain);
					% now let's identify what is the nature of the output
                    if isempty(remain_trimmed) % nothing after ']': we have a 1D list with no values, we will format it as a cell array of strings
                        arrStr = deblank(arrStr); % removes trailing spaces at end of each line of arrStr
                        arrStr = transpose(arrStr);
                        for i = 1:length(arrStr)-1 % last item is empty
                            arrStr{i}=pdApp.NumorStr(arrStr{i});
                        end
                        output = arrStr(1:length(arrStr)-1); % last item is empty
                    elseif remain(1)=='[' % we have '][': possibly a 2D array
                        next = strfind(remain,']'); % first ']'
                        idx = remain(2:next-1);
                        if isempty(str2num(idx)) % tests whether we have a number between the second brackets
                            output=pdApp.NumorStr(message); % not a number: not a 2D array
                        else
                            % this is a 2D array
							% check whether array starts with zero - in which case we need to add 1 to index
							pos1 = strfind(arrStr{1},'[');
                            pos2 = strfind(arrStr{1},']');
							idx1 = eval(arrStr{1}(pos1(1)+1:pos2(1)-1)); % first index of first item
                            idx2 = eval(arrStr{1}(pos1(2)+1:pos2(2)-1)); % second index of first item
							if idx1*idx2==0
								increment=1; % the index starts at zero => add 1
							else
								increment=0; % the index does not start at zero => should be OK as it is
							end
                            % handle contents
							for i = 1:length(arrStr)-1 % last item is empty
                                pos1 = strfind(arrStr{i},'[');
                                pos2 = strfind(arrStr{i},']');
                                idx1 = eval(arrStr{i}(pos1(1)+1:pos2(1)-1))+increment; % +1 as arrays start at zero in software
                                idx2 = eval(arrStr{i}(pos1(2)+1:pos2(2)-1))+increment; % +1 as arrays start at zero in software
                                arrStr{i}=arrStr{i}(pos2(2)+1:length(arrStr{i}));
                                arrStr{i}=pdApp.NumorStr(arrStr{i});
                                output{idx1,idx2}=arrStr{i};
                            end
                        end
                    elseif isspace(remain(1)) % there is a space after ']': 1D array, we will format it as a cell array
						for i = 1:length(arrStr)-1 % last item is empty
                            pos = strfind(arrStr{i},']');
                            arrStr{i}=arrStr{i}(pos+1:length(arrStr{i}));
                            arrStr{i}=pdApp.NumorStr(arrStr{i});
						end
                        arrStr = transpose(arrStr);
						output = arrStr(1:length(arrStr)-1); % last item is empty
                    end
                end
            end
        end
        function output = NumorStr(data)
            % removes space at front of output, checks whether it is a
            % string, a real number or a complex number
            % remove spaces at front
            idx = 1;
            while isspace(data(idx))
                idx = idx + 1;
            end
            data = data(idx:length(data));
            % test (real,complex)
            if data(1) == '('
                reidx = strfind(data,',');
                imidx = strfind(data,')');
                rebit = str2num(data(2:reidx-1));
                imbit = str2num(data(reidx+1:imidx-1));
                if isempty(rebit)
                    output = data;
                elseif isempty(imbit)
                    output = data;
                else
                    output = rebit + imbit*1i;
                end
            else
                % test real number
                bit = str2num(data);
                if isempty(bit) % string
					data = deblank(data); % remove trailing spaces
                    output = data;
				else % real number
                    output = bit;
                end
            end
        end
    end
end



