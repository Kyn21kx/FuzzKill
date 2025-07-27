const std = @import("std");
const zcc = @import("compile_commands");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});

    const exe = b.addExecutable(.{ .name = "FuzzKill", .target = target });

    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);
    targets.append(exe) catch @panic("OOM");

    // exe.addCSourceFile(.{ .file = b.path("src/main.cpp"), .flags = , .language = .cpp });
    const compileFlags = &[_][]const u8{ "-std=c++20", "-Wno-reorder" };
    const sources = &[_][]const u8{ "main.cpp", "Application.cpp", "FuzzKillUI.cpp" };
    exe.addCSourceFiles(.{ .files = sources, .flags = compileFlags, .language = .cpp, .root = b.path("src/") });

    zcc.createStep(b, "cdb", targets.toOwnedSlice() catch @panic("OOM"));

    exe.addIncludePath(b.path("third_party/clay/include/"));
    exe.addIncludePath(b.path("third_party/raylib/include/"));

    exe.linkSystemLibrary("raylib");
    exe.addLibraryPath(b.path("third_party/raylib/lib"));
    exe.linkLibCpp();

    b.installArtifact(exe);
}
