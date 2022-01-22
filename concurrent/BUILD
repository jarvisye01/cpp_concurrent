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
        "//util:jref",
        "//file:jfile",
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

cc_library(
    name = "jtask",
    srcs = [
        "jtask.cpp",
    ],
    hdrs = [
        "jtask.hpp",
    ],
    deps = [
        "//concurrent:jmutex",
        "//concurrent:jsyncqueue",
        "//concurrent:jthread",
        "//util:jlog",
        "//util:jconfig",
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
    name = "jshm",
    srcs = [
        "jshm.cpp",
        "jshm.hpp",
    ],
    hdrs = [
        "jshm.hpp",
    ],
    deps = [
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "concurrent_test",
    srcs = [
        "concurrent_test.cpp",
    ],
    deps = [
        "//concurrent:jthread",
        "//concurrent:jmutex",
        "//concurrent:jshm",
        "//file:jfile",
        "//util:jtime",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [
        "-lpthread",
    ],
)

cc_binary(
    name = "jtask_test",
    srcs = [
        "jtask_test.cpp",
    ],
    deps = [
        "//util:jlog",
        "//concurrent:jtask",
    ],
    copts = [
        "-DTEST_TASK",
        "-std=c++11",
    ],
    linkopts = [
        "-lpthread",
    ],
)
