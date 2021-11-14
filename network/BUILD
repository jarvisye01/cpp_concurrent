cc_library(
    name = "jconn",
    srcs = [
        "jconn.cpp",
    ],
    hdrs = [
        "jconn.hpp",
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
    name = "jnetutil",
    srcs = [
        "jnetutil.cpp",
    ],
    hdrs = [
        "jnetutil.hpp",
    ],
    deps = [
    ],
    copts = [
        "-std=c++20",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jeventloop",
    srcs = [
        "jeventloop.cpp",
    ],
    hdrs = [
        "jeventloop.hpp",
    ],
    deps = [
        "//network:jconn",
        "//util:jlog",
    ],
    copts = [
        "-std=c++20",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jserver",
    srcs = [
        "jserver.cpp",
    ],
    hdrs = [
        "jserver.hpp",
    ],
    deps = [
        "//network:jconn",
        "//network:jeventloop",
        "//concurrent:jthread",
        "//network:jnetutil",
        "//util:jrand",
        "//util:jlog",
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
    name = "jserver_test",
    srcs = [
        "jserver_test.cpp",
    ],
    deps = [
        "//network:jserver",
        "//util:jlog",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [
        "-lpthread",
    ],
)

cc_binary(
    name = "jclient_test",
    srcs = [
        "jclient_test.cpp",
    ],
    deps = [
        "//network:jnetutil",
    ],
    copts = [
        "-std=c++20",
    ],
)