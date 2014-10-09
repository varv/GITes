import pdMatlabLib.*
f = pdApp();
f.ConnectToApp();
T=[];

for i = 0:5
	h = i/10;
	f.Exec(['app.subnodes[1].subnodes[1].fsdevice.objects[1].setposition(' num2str(1.100000+h) ',0.000000)']);
	f.Exec(['app.subnodes[1].subnodes[1].fsdevice.objects[5].setposition(' num2str(1.300000+h) ',0.000000)']);
	f.Exec('app.subnodes[1].subnodes[1].fsdevice.calculatefdtd()');
	f.Exec('app.subnodes[1].subnodes[1].fsdevice.clearfdtd()');
	f.Exec('Set flux=app.subnodes[1].subnodes[1].fsdevice.getpolarisedfluxfdtd(2.000000,5,2,4,2,0)');
	T{length(T)+1,1} = f.Exec('flux');
end

disp(T);

clear f;
