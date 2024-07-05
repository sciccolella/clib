import sys
import re
from collections import defaultdict
import statistics


def load_file(filepath):
    mcd = defaultdict(list)
    with open(filepath, "r") as fin:
        for line in fin:
            m = re.match(r"\[(.*)\]\s+time:\s+(\d+):(\d+)", line)
            if m:
                # print(m.group(1), m.group(2), m.group(3))
                mcd[m.group(1)].append(int(m.group(2)) * 1000000 + int(m.group(3)))
            else:
                exit(1)
    return mcd


def str_Sms(ms):
    s = ms // 1000000
    return f"{s:2}:{ms%1000000:6}"


mc0 = load_file(sys.argv[1])
mc1 = load_file(sys.argv[2])
for m in mc0:
    print(
        m,
        str_Sms(int(statistics.mean(mc0[m]))),
        str_Sms(int(statistics.mean(mc1[m]))),
        sep="\t",
    )
