cc_library(
    name = "jfile",
    srcs = [
        "jfile.cpp",
    ],
    hdrs = [
        "jfile.hpp",
    ],
    deps = [
    ],
    copts = [
        "-std=c++11",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "jfile_test",
    srcs = [
        "jfile_test.cpp",
    ],
    deps = [
        "jfile",
    ],
    copts = [
        "-DTEST_COPY_FILE",
        "-std=c++11",
    ],
    linkopts = [],
)
