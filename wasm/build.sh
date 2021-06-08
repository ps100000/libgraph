./build_tools/resource2c/resource2c ./src/common/resource2c.cpp ./resources/*.glsl
echo "building wasm file & glue js"
em++																								\
	--bind																							\
	--shell-file ./build_tools/html_template.html													\
	--embed-file ./resources/binary/																\
	--std=c++17																						\
	-I /usr/include/glm/ -I src/																	\
	-s ENVIRONMENT=web -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy										\
	-s USE_SDL=2 -s USE_SDL_TTF=2																	\
	-s USE_GLFW=2 -s FULL_ES3=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s GL_ASSERTIONS=1	\
	-lSDL2 -lSDL2_ttf -lGL																			\
	-O2 -DNDEBUG  																					\
	-o bin/graph.html																				\
	src/graph.cpp																					\
	src/common/*.cpp																				\
	src/gl/*.cpp																					\
	src/graph/*.cpp																					\
	src/input/*.cpp																					\
	src/js_interface/*.cpp
echo "creating patched version of glue js for lib"
echo 'export function init(Module, wasmBinaryFile){' > bin/angular_graph.js
sed -E "s/var wasmBinaryFile ?= ?[\"']graph.wasm[\"'];//" bin/graph.js >> bin/angular_graph.js
echo '}' >> bin/angular_graph.js

echo "copying/moving files that are needed for lib"
mv bin/angular_graph.js ../angular-graph-lib/projects/graph-lib/src/lib/
cp bin/graph.wasm ../angular-graph-lib/projects/graph-lib/assets/wasm/
echo "done!"
