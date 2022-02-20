cc_binary(
    name = "jminikv",
    srcs = [
        "main.cpp",
        "minikv.cpp",
        "minikv.hpp",
    ],
    deps = [
        "//concurrent:jsyncmap",
        "//network:jserver",
        "//network:jparser",
        "//network:jpoller",
        "//network:jeventloop",
        "//network:jsocket",
    ],
    copts = [
        "-DTEST_COPY_FILE",
        "-std=c++11",
        "-g",
    ],
    linkopts = [],
)

cc_binary(
    name = "jminikv_cli",
    srcs = [
        "cli_main.cpp",
    ],
    deps = [
        "//network:jparser",
        "//network:jsocket",
    ],
    copts = [
        "-DTEST_COPY_FILE",
        "-std=c++11",
        "-g",
    ],
    linkopts = [],
)
