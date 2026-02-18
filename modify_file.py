import sys
import re

with open("tools/XenosRecomp/XenosRecomp/shader_recompiler.cpp", "r") as f:
    lines = f.readlines()

new_lines = []
for i, line in enumerate(lines):
    # Hunk 1 (1308)
    if line.strip() == "};" and "VertexElement vertexElement;" in lines[i-2]:
        new_lines.append(line.replace("};", "} u;"))
    elif "value = vertexShader->vertexElementsAndInterpolators" in line:
        new_lines.append(line.replace("value =", "u.value ="))
    elif "const char* usageType = USAGE_TYPES" in line:
        new_lines.append(line.replace("vertexElement.usage", "u.vertexElement.usage"))
    elif "if ((vertexElement.usage ==" in line:
        new_lines.append(line.replace("vertexElement", "u.vertexElement"))
    elif "(vertexElement.usage == DeclUsage::Position" in line:
        new_lines.append(line.replace("vertexElement", "u.vertexElement"))
    elif "if (usageLocation.usage == vertexElement.usage" in line:
        new_lines.append(line.replace("vertexElement", "u.vertexElement"))
    elif 'println("in {0} i{1}{2} : {3}{2},"' in line:
        new_lines.append(line.replace("vertexElement", "u.vertexElement"))
    elif 'uint32_t(vertexElement.usageIndex), USAGE_SEMANTICS' in line:
        new_lines.append(line.replace("vertexElement", "u.vertexElement"))
    elif 'vertexElements.emplace(uint32_t(vertexElement.address), vertexElement);' in line:
        # vertexElements is map, vertexElement is struct
        # replace vertexElement.address -> u.vertexElement.address
        # replace second vertexElement -> u.vertexElement
        l = line.replace("vertexElement.address", "u.vertexElement.address")
        l = l.replace(", vertexElement);", ", u.vertexElement);")
        new_lines.append(l)

    # Hunk 2 (1403)
    elif line.strip() == "};" and "int8_t w;" in lines[i-1]:
        new_lines.append(line.replace("};", "} s;"))
    elif '(definition->registerIndex - 8992) / 4 + i, x, y, z, w);' in line:
        new_lines.append(line.replace("x, y, z, w", "s.x, s.y, s.z, s.w"))

    # Hunk 3 (1502)
    elif line.strip() == "};" and "uint32_t code3;" in lines[i-1]:
        new_lines.append(line.replace("};", "} codes;"))

    # Hunk 4 (1512) & 4b (1579)
    elif "code0 = controlFlowCode[0];" in line:
        new_lines.append(line.replace("code0", "codes.code0"))
    elif "code1 = controlFlowCode[1] & 0xFFFF;" in line:
        new_lines.append(line.replace("code1", "codes.code1"))
    elif "code2 = (controlFlowCode[1] >> 16) | (controlFlowCode[2] << 16);" in line:
        new_lines.append(line.replace("code2", "codes.code2"))
    elif "code3 = controlFlowCode[2] >> 16;" in line:
        new_lines.append(line.replace("code3", "codes.code3"))

    # Hunk 5 (1733)
    elif line.strip() == "};" and "uint32_t code2;" in lines[i-1] and "uint32_t code1;" in lines[i-2]:
        if "uint32_t code3;" in lines[i-1]:
             new_lines.append(line)
        else:
             new_lines.append(line.replace("};", "} raw;"))

    elif "code0 = instructionCode[0];" in line:
        new_lines.append(line.replace("code0", "raw.code0"))
    elif "code1 = instructionCode[1];" in line:
        new_lines.append(line.replace("code1", "raw.code1"))
    elif "code2 = instructionCode[2];" in line:
        new_lines.append(line.replace("code2", "raw.code2"))

    else:
        new_lines.append(line)

with open("tools/XenosRecomp/XenosRecomp/shader_recompiler.cpp", "w") as f:
    f.writelines(new_lines)
