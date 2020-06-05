// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gn/rust_project_writer_helpers.h"

#include "base/strings/string_util.h"
#include "gn/filesystem_utils.h"
#include "gn/string_output_buffer.h"
#include "gn/test_with_scheduler.h"
#include "gn/test_with_scope.h"
#include "util/build_config.h"
#include "util/test/test.h"

using RustProjectWriterHelper = TestWithScheduler;

TEST_F(RustProjectWriterHelper, WriteCrates) {
  TestWithScope setup;

  CrateList crates;
  Crate dep = Crate(SourceFile("/root/tortoise/lib.rs"), 0, "//tortoise:bar", "2015");
  Crate target = Crate(SourceFile("/root/hare/lib.rs"), 1, "//hare:bar", "2015");
  target.AddDependency(0, "tortoise");
  target.AddConfigItem("unix");
  target.AddConfigItem("feature=\\\"test\\\"");

  crates.push_back(dep);
  crates.push_back(target);

  std::ostringstream stream;
  WriteCrates(setup.build_settings(), crates, stream);
  std::string out = stream.str();
#if defined(OS_WIN)
  base::ReplaceSubstringsAfterOffset(&out, 0, "\r\n", "\n");
#endif
  const char expected_json[] =
      "{\n"
      "  \"roots\": [\n"
      "    \"/root/tortoise/\",\n"
      "    \"/root/hare/\"\n"
      "  ],\n"
      "  \"crates\": [\n"
      "    {\n"
      "      \"crate_id\": 0,\n"
      "      \"root_module\": \"/root/tortoise/lib.rs\",\n"
      "      \"label\": \"//tortoise:bar\",\n"
      "      \"deps\": [\n"
      "      ],\n"
      "      \"edition\": \"2015\",\n"
      "      \"cfg\": [\n"
      "      ]\n"
      "    },\n"
      "    {\n"
      "      \"crate_id\": 1,\n"
      "      \"root_module\": \"/root/hare/lib.rs\",\n"
      "      \"label\": \"//hare:bar\",\n"
      "      \"deps\": [\n"
      "        {\n"
      "          \"crate\": 0,\n"
      "          \"name\": \"tortoise\"\n"
      "        }\n"
      "      ],\n"
      "      \"edition\": \"2015\",\n"
      "      \"cfg\": [\n"
      "        \"unix\",\n"
      "        \"feature=\\\"test\\\"\"\n"
      "      ]\n"
      "    }\n"
      "  ]\n"
      "}\n";

  EXPECT_EQ(expected_json, out);
}

TEST_F(RustProjectWriterHelper, SysrootDepsAreCorrect) {
  TestWithScope setup;
  setup.build_settings()->SetRootPath(UTF8ToFilePath("/root"));

  SysrootIndexMap sysroot_lookup;
  CrateList crates;

  AddSysroot(setup.build_settings(), "sysroot", sysroot_lookup, crates);

  std::ostringstream stream;
  WriteCrates(setup.build_settings(), crates, stream);
  std::string out = stream.str();
#if defined(OS_WIN)
  base::ReplaceSubstringsAfterOffset(&out, 0, "\r\n", "\n");
#endif

const char expected_json[] =
    "{\n"
    "  \"roots\": [\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libcore/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/liballoc/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libpanic_abort/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libunwind/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libstd/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libcollections/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/liblibc/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libpanic_unwind/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libproc_macro/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_unicode/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libstd_unicode/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libtest/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/liballoc_jemalloc/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/liballoc_system/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libcompiler_builtins/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libgetopts/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libbuild_helper/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_asan/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_lsan/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_msan/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_tsan/\",\n"
    "    \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libsyntax/\"\n"
    "  ],\n"
    "  \"crates\": [\n"
    "    {\n"
    "      \"crate_id\": 0,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libcore/lib.rs\",\n"
    "      \"label\": \"core\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 1,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/liballoc/lib.rs\",\n"
    "      \"label\": \"alloc\",\n"
    "      \"deps\": [\n"
    "        {\n"
    "          \"crate\": 0,\n"
    "          \"name\": \"core\"\n"
    "        }\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 2,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libpanic_abort/lib.rs\",\n"
    "      \"label\": \"panic_abort\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 3,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libunwind/lib.rs\",\n"
    "      \"label\": \"unwind\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 4,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libstd/lib.rs\",\n"
    "      \"label\": \"std\",\n"
    "      \"deps\": [\n"
    "        {\n"
    "          \"crate\": 1,\n"
    "          \"name\": \"alloc\"\n"
    "        },\n"
    "        {\n"
    "          \"crate\": 0,\n"
    "          \"name\": \"core\"\n"
    "        },\n"
    "        {\n"
    "          \"crate\": 2,\n"
    "          \"name\": \"panic_abort\"\n"
    "        },\n"
    "        {\n"
    "          \"crate\": 3,\n"
    "          \"name\": \"unwind\"\n"
    "        }\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 5,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libcollections/lib.rs\",\n"
    "      \"label\": \"collections\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 6,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/liblibc/lib.rs\",\n"
    "      \"label\": \"libc\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 7,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libpanic_unwind/lib.rs\",\n"
    "      \"label\": \"panic_unwind\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 8,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libproc_macro/lib.rs\",\n"
    "      \"label\": \"proc_macro\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 9,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_unicode/lib.rs\",\n"
    "      \"label\": \"rustc_unicode\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 10,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libstd_unicode/lib.rs\",\n"
    "      \"label\": \"std_unicode\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 11,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libtest/lib.rs\",\n"
    "      \"label\": \"test\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 12,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/liballoc_jemalloc/lib.rs\",\n"
    "      \"label\": \"alloc_jemalloc\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 13,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/liballoc_system/lib.rs\",\n"
    "      \"label\": \"alloc_system\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 14,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libcompiler_builtins/lib.rs\",\n"
    "      \"label\": \"compiler_builtins\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 15,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libgetopts/lib.rs\",\n"
    "      \"label\": \"getopts\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 16,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libbuild_helper/lib.rs\",\n"
    "      \"label\": \"build_helper\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 17,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_asan/lib.rs\",\n"
    "      \"label\": \"rustc_asan\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 18,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_lsan/lib.rs\",\n"
    "      \"label\": \"rustc_lsan\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 19,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_msan/lib.rs\",\n"
    "      \"label\": \"rustc_msan\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 20,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/librustc_tsan/lib.rs\",\n"
    "      \"label\": \"rustc_tsan\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 21,\n"
    "      \"root_module\": \"/root/out/Debug/sysroot/lib/rustlib/src/rust/src/libsyntax/lib.rs\",\n"
    "      \"label\": \"syntax\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "      ]\n"
    "    }\n"
    "  ]\n"
    "}\n";
;
  EXPECT_EQ(expected_json, out);
}
