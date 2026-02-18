with open("patches/xenos_recomp_fixes.patch", "r") as f:
    original_lines = f.readlines()

# Extract CMakeLists.txt part (lines 0 to 13 approx)
cmake_part = []
for line in original_lines:
    if line.startswith("diff --git a/XenosRecomp/shader_recompiler.cpp"):
        break
    cmake_part.append(line)

with open("my_shader.patch", "r") as f:
    shader_part = f.readlines()

with open("patches/xenos_recomp_fixes.patch", "w") as f:
    f.writelines(cmake_part)
    f.writelines(shader_part)
