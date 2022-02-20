cc_library(
    name = "jtime",
    srcs = [
        "jtime.cpp",
    ],
    hdrs = [
        "jtime.hpp",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jbuffer",
    srcs = [
        "jbuffer.cpp",
    ],
    hdrs = [
        "jbuffer.hpp",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jlog",
    srcs = [
        "jlog.cpp",
    ],
    hdrs = [
        "jlog.hpp",
    ],
    deps = [
        "//concurrent:jmutex",
        "//concurrent:jthread",
        "//concurrent:jsyncqueue",
        "//util:jbuffer",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jrand",
    srcs = [
        "jrand.cpp",
    ],
    hdrs = [
        "jrand.hpp",
    ],
    deps = [
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [
        "-lpthread",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jstring",
    srcs = [
        "jstring.cpp",
    ],
    hdrs = [
        "jstring.hpp",
    ],
    deps = [
        "//util:jrand",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jconfig",
    srcs = [
        "jconfig.cpp",
    ],
    hdrs = [
        "jconfig.hpp",
    ],
    deps = [
        "//file:jfile",
        "//util:jstring",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jref",
    srcs = [
        "jref.cpp",
    ],
    hdrs = [
        "jref.hpp",
    ],
    deps = [
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jdebug",
    srcs = [
        "jdebug.cpp",
    ],
    hdrs = [
        "jdebug.hpp",
    ],
    deps = [
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "jutil_test",
    srcs = [
        "jutil_test.cpp",
    ],
    deps = [
        "//concurrent:jmutex",
        "//concurrent:jthread",
        "//concurrent:jsyncqueue",
        "//util:jbuffer",
        "//util:jlog",
        "//util:jrand",
        "//util:jstring",
        "//util:jconfig",
        "//util:jref",
        "//util:jdebug",
    ],
    copts = [
        "-std=c++20",
        # "-DJCONFIG",
         "-DJREF",
        # "-DJNEWBUFFER",
    ],
    linkopts = [
        "-lpthread",
    ],
)

cc_binary(
    name = "jbuffer_test",
    srcs = [
        "jbuffer_test.cpp",
    ],
    deps = [
        "//util:jbuffer",
        "//util:jrand",
        "//util:jstring",
        "//util:jtime",
        "//file:jfile",
    ],
    copts = [
        "-std=c++11",
        "-DJCONFIG",
        "-DJREF",
    ],
    linkopts = [
    ],
)
