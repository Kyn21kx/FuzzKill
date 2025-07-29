const std = @import("std");
const zcc = @import("compile_commands");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});

    const exe = b.addExecutable(.{ .name = "FuzzKill", .target = target });

    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);
    targets.append(exe) catch @panic("OOM");

    // Use clay as a library so its behavior is self contained
    const clayLibRendering = b.addStaticLibrary(.{ .name = "clay", .target = target });
    clayLibRendering.addCSourceFile(.{ .file = b.path("src/renderer/clay_renderer_raylib.c"), .flags = &[_][]const u8{"-std=c++20"}, .language = .cpp });
    clayLibRendering.addIncludePath(b.path("third_party/clay/include/"));
    clayLibRendering.addIncludePath(b.path("third_party/raylib/include/"));
    clayLibRendering.linkSystemLibrary("c");

    const compileFlags = &[_][]const u8{ "-std=c++20", "-Wno-reorder" };
    const sources = &[_][]const u8{ "main.cpp", "Application.cpp", "FuzzKillUI.cpp", "utils/ProcessLayer.cpp" };
    exe.addCSourceFiles(.{ .files = sources, .flags = compileFlags, .language = .cpp, .root = b.path("src/") });

    zcc.createStep(b, "cdb", targets.toOwnedSlice() catch @panic("OOM"));

    exe.addIncludePath(b.path("third_party/clay/include/"));
    exe.addIncludePath(b.path("third_party/raylib/include/"));
    exe.addIncludePath(b.path("third_party/zadeh/include/"));

    exe.linkLibrary(clayLibRendering);
    exe.linkSystemLibrary("raylib");
    exe.addLibraryPath(b.path("third_party/raylib/lib"));
    exe.linkLibCpp();
    exe.linkSystemLibrary("c");

    b.installArtifact(exe);
}
