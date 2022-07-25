cmake_minimum_required(VERSION 2.8)

project(tprender C)

SET(CROSS_COMPILE 1)
set(CMAKE_SYSTEM_NAME Linux)
#编译器路径
set(CMAKE_C_COMPILER "/home/book/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/bin/riscv64-unknown-linux-gnu-gcc")
#链接库路径
link_directories(
		./libs
		/home/book/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/lib/
)

aux_source_directory(src SOURCE)

include_directories(./)
include_directories(./include)
include_directories(./glib-2.0)
#头文件路径
include_directories(/home/book/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/include/)
include_directories(/home/book/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/include/allwinner/)
include_directories(/home/book/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/include/allwinner/include)

link_libraries(
		libglib-2.0.so
		libpcre.so
		pthread
		libgobject-2.0.so
		libgmodule-2.0.so
		libm.so
		libffi.so
		libupnp.so
		libixml.so
		
		libtplayer.so
		libxplayer.so
		libvdecoder.so
		libadecoder.so
		libsubdecoder.so
		libcdc_base.so
		libVE.so
		libMemAdapter.so
		libcdx_parser.so
		libcdx_playback.so
		libcdx_stream.so
		libcdx_base.so
		libpthread.so
		libuapi.so
		libz.so
		libasound.so
		libssl.so
		libsbm.so
		libaftertreatment.so
		libscaledown.so
		libfbm.so
		libvideoengine.so
		libcdx_common.so
		libcrypto.so
)

add_executable(tprender
			${SOURCE}
)
