objs	= 	button.obj font.obj font8x8.obj framebuffer.obj input.obj main.obj movement.obj \
			point.obj point3d.obj rasterize.obj sceneobject.obj screen.obj serial.obj \
			shapes.obj timer.obj triangle.obj vector3f.obj widget.obj
			
cpp_flags = -i="C:\WATCOM/h" -i="C:\Users\Luigi\Documents\watcom\polygon\include" -w4 -e25 -zq -od -d2 -6r -bt=dos -fo=.obj -mf -xs -xr

.cpp.obj
	wpp386 $<
	
polygon.exe : $(objs)