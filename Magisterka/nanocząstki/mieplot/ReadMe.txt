MiePlot v4.3

In 1908, Gustav Mie developed a rigorous method to calculate the intensity of light scattered by uniform spheres.  Although Mie's solution was precise, it involved a huge number of calculations for large spheres and was rarely used until the 1980s when supercomputers became available for scientific research.  

The rapid advances in computer power over the last few years allow Mie scattering calcuations on standard personal computers. The MiePlot computer program uses the Mie algorithm to produce full-colour simulations of atmospheric optical effects, such as rainbows, coronas and glories.  Such simulations can be superimposed on photographs of natural effects.  

MiePlot offers the following mathematical models for the scattering of light by a sphere: 

     Mie theory
     Debye series
     Ray tracing (based on geometrical optics) 
     Ray tracing including the effects of interference between rays 
     Airy theory 
     Rayleigh scattering 
     Diffraction 
     Surface waves

MiePlot can plot graphs of scattered intensity as a function of scattering angle, radius, wavelength, refractive index - as well as "Lee diagrams". In addition, it can produce graphs of scattering cross-sections (Cext, Csca & Cabs) and scattering efficiencies (Qext, Qsca & Qabs) as functions of radius of the scattering sphere, size parameter or wavelength.  
Many examples of MiePlot's graphical outputs and simulations of atmospheric optical effects are available at www.philiplaven.com.. 

Please consult the Help file for detailed instructions about using the many features of MiePlot.

If you experience any problems using MiePlot or have any suggestions for new facilities, please send an e-mail to the author, Philip Laven, at philip@philiplaven.com


___________________________________________________________________________

Installation instructions

The latest version of MiePlot can be downloaded from http://www.philiplaven.com/mieplot.htm

Having downloaded MiePlot zip file, you should extract the archived files into an appropriate directory on your computer (e.g. C:\Program Files\MiePlot).  The following files should be located in the same directory:

    MiePlot v4300.exe (or a subsequent version, such as MiePlot v4304.exe)
    MiePlot.hlp		    	Help file
    MiePlot.cnt		    	Help contents file 
    ReadMe.txt		    	This file 
    Segelstein.txt	    	Data file on the refractive index for water
    spectrum.spe	    	Sample data file for spectrum of incident light
    gold.rix		    	Sample file for refractive index of gold
    Polystyrene.rix		Sample file for refractive index of gold
    Normal_R5_SD20.dis      	Sample file for Normal distribution of radius of spheres
    LogNormal_R5_SD20.dis   	Sample file for Log-Normal distribution of radius of spheres
    Input_1.min	            	Sample file for batch operations
    Dispersion.rxi		Data file for Sellmeier coefficients for refractive index (do NOT edit manually)
    DispersionBackup.rxi	Backup file for Dispersion.rxi (do NOT edit manually)

To run MiePlot, simply run MiePlot v4300.exe by double clicking on this file using Windows Explorer. 

Note that MiePlot was originally designed for displays with 1024 x 768 pixels.  Although the basic features work on 800 x 600 displays, 1024 x 768 displays give much better results.  If you are still using a display with 640 x 480 pixels, MiePlot will not be usable at this resolution - sorry!  

For displays using more than 1024 x 768 pixels, MiePlot now adjusts the layout to fit the screen.  This feature has been tested on various displays up to 1600 x 1200 pixels.  Please let me know if you experience any problems with this new feature.

_________________________________________________________________________

Microsoft Windows: International settings

Users of PCs configured in languages other than English may find that MiePlot fails to start properly giving an error message headed “Microsoft Windows: problem with International Settings”.  This problem is caused by these versions of Windows expecting “,” (rather than “.”) as the decimal symbol.  For example, MiePlot assumes that numbers will be entered in the form “1.25”, not “1,25”.  In fact, error-checking routines will automatically convert such entries into the form expected by MiePlot. 

To overcome this problem in Windows 2000, you should select “Settings” from the Start menu, followed by “Control Panel”, “Regional options” and then under “Numbers” select “.” as the “Decimal symbol”.

To overcome this problem in Windows XP, you should select “Settings” from the Start menu, followed by “Control Panel”, “Regional and language options” and then select “.” as the “Decimal symbol”.

To overcome this problem in Windows Vista, you should select "Settings" from the Start menu, followed by "Control Panel" and "Regional and language options".  Under the "Formats" tab, you should select "Customize this format" and and then select "." as the "Decimal symbol".

To overcome this problem in Windows 7, you should select "Control Panel" from the Start menu, followed by "Region and language".  Under the "Formats" tab, you should select "Additional settings" and and then select "." as the "Decimal symbol".

_________________________________________________________________________

N.B. Missing Files 

As MiePlot was written in Visual Basic, some additional files may need to be installed on your PC. 

If you see an error message regarding MSVBVM60.DLL, you may need to download MSVBVM60.DLL and put it into your Windows system directory (e.g. in C:\Windows\System or C:\Windows\System32).  Alternatively, you can install all of the Microsoft Visual Basic 6.0 run-time files by downloading and running VBRUN6.EXE (or VBRUN60.EXE).  This file is available from Microsoft or from many other sites. 

A good source of information is http://www.webattack.com/help/missingfiles.html where you will find many direct links to the "missing files" plus helpful installation instructions. 


Many modern PCs already include the required files, but please send an e-mail to philip@philiplaven.com if you have any problems installing or running MiePlot.

_________________________________________________________________________
