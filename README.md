# QT5DX11 
DirectX11 rendering in QT5 window without losing other widgets

The main purpuse of this test is to be able to use standard controls on top of DX rendering. 
If you simply grab the HWND and start rendering, everything already existing in that window will be rendered on.

With the help from @don_uorri I have created two methods, neither is perfect but depending on what's going to be rendered it can be OK.

METHOD 1 - Works but slow
Get HWND from a QFRAME instead of the main window.
Convert the DX rendering to a texture, convert it to a QBitmap and draw it in the qframe paintEvent
qframe has to be updated each frame

METHOD 2 - Almost works, fast but can produce flicker
Render DX on qframe, place controls on top of qframe (not in)
If you move the mouse between the buttons like crazy a flicker occurs as a box between them, when vsync is ON.
When VSync is ON, this method is actually slower on my machine. This is of course because nothings being rendered.
I have not tried doing any heavy rendering here but I suspect that if the fps drops to around 60, the flickering will be visible even without vsync.
