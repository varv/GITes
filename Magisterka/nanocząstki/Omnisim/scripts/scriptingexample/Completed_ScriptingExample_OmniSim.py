from pdPythonLib import *
f = pdApp()
f.ConnectToApp()

for i in range (0,5,1):
	h = i/10.
	f.Exec("app.subnodes[1].subnodes[1].fsdevice.objects[1].setposition({1.100000+h},0.000000)")
	f.Exec("app.subnodes[1].subnodes[1].fsdevice.objects[5].setposition({1.300000+h},0.000000)")
	f.Exec("app.subnodes[1].subnodes[1].fsdevice.calculatefdtd()")
	f.Exec("app.subnodes[1].subnodes[1].fsdevice.clearfdtd()")
	f.Exec("Set flux=app.subnodes[1].subnodes[1].fsdevice.getpolarisedfluxfdtd(2.000000,5,2,4,2,0)")
	T = f.Exec("flux")
	print T

raw_input(">>>>")
del f
