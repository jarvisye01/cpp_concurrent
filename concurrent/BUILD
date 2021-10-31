cc_library(
    name = "jmutex",
    srcs = [
        "jmutex.cpp",
    ],
    hdrs = [
        "jmutex.hpp",
    ],
    deps = [
        "//util:jtime",
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
    name = "jthread",
    srcs = [
        "jthread.cpp",
    ],
    hdrs = [
        "jthread.hpp",
    ],
    deps = [
        "//util:jtime",
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
    name = "jsyncqueue",
    srcs = [
        "jqueue.cpp",
    ],
    hdrs = [
        "jqueue.hpp",
    ],
    deps = [
        "//concurrent:jmutex",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [
        "-lpthread",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "concurrent_test",
    srcs = [
        "concurrent_test.cpp",
    ],
    deps = [
        ":jthread",
        "jmutex",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [
        "-lpthread",
    ],
)