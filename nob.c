//
// nob.c - Build file
//
// Commands:
//   - build                         Builds TARGETS. (DEFAULT)
//   - run <TARGETS> [ARGS...]       Builds and runs selected target.
//
// TARGETS:
//   - mpp6-conventer
//   - mpp6
//

typedef enum {
  TARGET_MPP6_CONVENTER = 0,
  TARGET_MPP6,
  COUNT_TARGETS
} targets_t;

const char *targetNames[COUNT_TARGETS] = {
  [TARGET_MPP6_CONVENTER] = "mpp6-conventer",
  [TARGET_MPP6]           = "mpp6",
};

const char *targets[COUNT_TARGETS] = {
  [TARGET_MPP6_CONVENTER] = "mpp6/conventer.c",
  [TARGET_MPP6]           = "mpp6/main.c",
};

#define NOB_IMPLEMENTATION
#include "include/nob.h"
#include "include/utils.h"

size_t valid_target(const char *const target_name) {
  for (size_t i = 0; i < COUNT_TARGETS; ++i)
    if (strcmp(targetNames[i], target_name) == 0) return i;
  return COUNT_TARGETS;
}

bool build_target(const char *const dir_name, const char *const target) {
  size_t targetNo = COUNT_TARGETS;
  if ((targetNo = valid_target(target)) == COUNT_TARGETS) return false;

  Nob_String_Builder  build_sb = {0}; // <dir_name>build/<target_name>
  nob_sb_append_cstr(&build_sb, dir_name);
  nob_sb_append_cstr(&build_sb, "build/");
  nob_sb_append_cstr(&build_sb, target);
  nob_sb_append_null(&build_sb);

  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "gcc", "-o", build_sb.items);

  Nob_String_Builder  sb = {0}; // <dir_name><target_path>
  nob_sb_append_cstr(&sb, dir_name);
  nob_sb_append_cstr(&sb, targets[targetNo]);
  nob_sb_append_null(&sb);

  nob_cmd_append(&cmd, sb.items);

  if (!nob_cmd_run_sync(cmd)) {
    nob_log(NOB_ERROR, "Failed to build target `%s`!", target);
    return false;
  }

  nob_sb_free(sb);

  return true;
}

bool build_all(const char *const dir_name) {
  for (size_t i = 0; i < COUNT_TARGETS; ++i)
    if (!build_target(dir_name, targetNames[i])) return false;
  return true;
}

bool run_target(const char *const dir_name, const char *const target, int *argc, char ***argv) {
  if (valid_target(target) == COUNT_TARGETS) {
    nob_log(NOB_ERROR, "Invalid target `%s`!", target);
    return false;
  }

  Nob_String_Builder  dir_sb = {0}; // <dir_name>build/<target_name>
  nob_sb_append_cstr(&dir_sb, dir_name);
  nob_sb_append_cstr(&dir_sb, "build/");
  nob_sb_append_null(&dir_sb);

  char *file_name = find_file(target, dir_sb.items);
  if (!file_name) return false;

  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, file_name);
  while (*argc > 0) nob_cmd_append(&cmd, nob_shift_args(argc, argv));

  return nob_cmd_run_sync(cmd);
}

void usage(Nob_Log_Level level, const char *const program) {
  nob_log(level, "Usage: %s <COMMANDS>\n", program);
  nob_log(level, "COMMANDS:");
  nob_log(level, "  - build                         Builds TARGETS. (DEFAULT)");
  nob_log(level, "  - run <TARGETS> [ARGS...]       Builds and runs selected target.");
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  const char *const program = nob_shift_args(&argc, &argv);
  char *dir_name = get_dir_name(program);

  const char *cmd = NULL;
  if (argc <= 0) {
    nob_log(NOB_WARNING, "<COMMANDS> missing! (build choosen as defualt)");
    cmd = "build";
  } else {
    cmd = nob_shift_args(&argc, &argv);
  }

  nob_mkdir_if_not_exists("./build/");
  if (strcmp(cmd, "help") == 0) {
    nob_log(NOB_ERROR, "READ THE F*CKING SOURCE CODE!"); // No help for you.
    return 1;
  } else if (strcmp(cmd, "build") == 0) {
    return !build_all(dir_name);
  } else if (strcmp(cmd, "run") == 0) {
    if (argc <= 0) {
      nob_log(NOB_ERROR, "<TARGETS> missing!");
      usage(NOB_ERROR, program);
      return 1;
    }
    const char *const target = nob_shift_args(&argc, &argv);
    if (valid_target(target) == COUNT_TARGETS) {
      nob_log(NOB_ERROR, "Invalid target `%s`!", target);
      return 1;
    }
    if (!build_target(dir_name, target)) return 1;
    return !run_target(dir_name, target, &argc, &argv);
  }

  nob_log(NOB_ERROR, "<COMMANDS> missing!");
  usage(NOB_ERROR, program);
  return 1;
}