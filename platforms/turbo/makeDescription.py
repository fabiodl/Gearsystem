import re


def getNames():
    names = []
    blacklist = ("VL_MODULE", "VL_ATTR_ALIGNED", "Vtop", "VL_DEBUG")
    with open("mapper/obj_dir/Vtop.h") as f:
        lines = f.readlines()
        for line in lines:
            if "VL_" in line:
                blacklisted = False
                for b in blacklist:
                    if b in line:
                        blacklisted = True
                if blacklisted:
                    continue
                tokens = re.findall(r"[\w']+", line)
                names.append(tokens[1])
    return names


header = """
#include <turboz/StatePrinter.h>
#include <map>

class VtopPrinter{
public:
  VtopPrinter(Vtop& o);
  std::map<int,BulkPrinter> level;
};

VtopPrinter::VtopPrinter(Vtop& o){
"""

footer = "}"


def makeFile(names):
    with open("mapper/obj_dir/VtopPrinter.h", "w") as f:
        f.write(header)
        for n in names:
            dots = n.count("__DOT__") + n.count("__TOP__")
            f.write("  level[{}].push_back(\"{}\",o.{});\n".format(dots, n, n))
        f.write(footer)


makeFile(getNames())
