--------------------------------------------------------------
print("Building catdog_producer")

project("CatDogProducer")
	kind("SharedLib")
	language("C++")
	Platform_SetCppDialect()
	dependson { "AssetPipelineCore" }

	location(path.join(RootPath, "build"))

	filter { "configurations:Debug" }
		objdir(path.join(RootPath, "build/obj/Debug"))
		targetdir(path.join(RootPath, "build/bin/Debug"))
		libdirs(path.join(RootPath, "build/bin/Debug"))
		links {
			path.join(RootPath, "build/bin/Debug/AssetPipelineCore")
		}		
	filter { "configurations:Release" }
		objdir(path.join(RootPath, "build/obj/Release"))
		targetdir(path.join(RootPath, "build/bin/Release"))
		libdirs(path.join(RootPath, "build/bin/Release"))
		links {
			path.join(RootPath, "build/bin/Release/AssetPipelineCore")
		}		
	filter {}

	defines {
		"TOOL_BUILD_SHARED"
	}

	files {
		path.join(RootPath, "public/Producers/CatDogProducer/**.*"),
		path.join(RootPath, "private/Producers/CatDogProducer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "public/Producers/CatDogProducer/**.*"),
			path.join(RootPath, "private/Producers/CatDogProducer/**.*"),
		},
	}

	includedirs {
		path.join(RootPath, "public"),
		path.join(RootPath, "private"),
		path.join(RootPath, "external"),
	}