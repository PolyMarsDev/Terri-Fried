.PHONY: all
all: build-gamecube build-linux build-nds build-switch build-windows-raylib build-xbox 
.PHONY: build-gamecube
build-gamecube:
	@mkdir -p ~/bin
	@cd gamecube && make && mv gamecube.dol ~/bin/terri-fried.gamecube.dol && mv gamecube.elf ~/bin/terri-fried.gamecube.elf	
.PHONY: build-linux
build-linux: 
	@cd linux && make && mv build/bin/Terri-fried ~/bin/terri-fried.linux.x64
.PHONY: build-nds
build-nds:
	@cd nds && make clean && make && mv nds.elf ~/bin/terri-fried.nds.elf && mv nds.nds ~/bin/terri-fried.nds
.PHONY: build-psvita
build-psvita:
	@cd psvita && make
	#/usr/local/vitasdk/bin/../lib/gcc/arm-vita-eabi/10.1.0/../../../../arm-vita-eabi/bin/ld: /usr/local/vitasdk/bin/../lib/gcc/arm-vita-eabi/10.1.0/../../../../arm-vita-eabi/lib/libvita2d.a(vita2d.o): in function `vita2d_swap_buffers': (.text+0xd18): undefined reference to `sceSharedFbEnd'
.PHONY: build-switch
build-switch:
	@cd switch && make && mv Terri-Fried.elf ~/bin/terri-fried.switch.elf && mv Terri-Fried.nacp ~/bin/terri-fried.switch.nacp && mv Terri-Fried.nro ~/bin/terri-fried.switch.nro
.PHONY: build-wii
build-wii:
	@cd wii && GRRLIB=/opt/grrlib make && mv wii.elf ~/bin/terri-fried.wii.elf && mv wii.dol ~/bin/terri-fried.wii.dol
	#/opt/devkitpro/devkitPPC/bin/../lib/gcc/powerpc-eabi/10.2.0/../../../../powerpc-eabi/bin/ld: cannot find -lgrrlib
.PHONY: build-windows-raylib
build-windows-raylib:
	@cd windows/raylib && make && mv Terr-Fried ~/bin/terri-fried.windows.exe
.PHONY: build-windows-sdl2
build-windows-sdl2:
	@cd windows/sdl2 && make
	#/usr/bin/x86_64-w64-mingw32-ld: cannot find -lSDL2_Image
.PHONY: build-xbox
build-xbox:
	@cd xbox && make && mv bin/default.xbe ~/bin/terri-fried.xbox.xbe
.PHONY: build-docker-all
build-docker-all:
	@docker run -v ${PWD}:/src terrifried
.PHONY: docker-cli
docker-cli:
	@docker run -v ${PWD}:/src -it terrifried /bin/bash
