import numpy as np

with open("taglog.txt", "r") as file:
    after_tags = []
    before_tags = []
    threshold = 1.0

    while True:
        l = file.readline()

        # break if EOF
        if not l:
            break

        if l.startswith("Tag Debug: BEFORE"):
            file.readline() # skip the "Input Stream" line
            l = file.readline()
            while "Offset" in l:
                num_str = l[l.find("{")+1:l.find("}")].split(" ")
                num_fmt = [num.replace("i", "j").replace("+-","-") for num in num_str]
                num_cplx = [complex(num) for num in num_fmt]
                if all(np.abs(np.array(num_cplx)) > threshold):
                    before_tags.append([complex(num) for num in num_fmt])
                l = file.readline()

        if l.startswith("Tag Debug: AFTER"):
            file.readline()
            l = file.readline()
            while "Offset" in l:
                num_str = l[l.find("{") + 1:l.find("}")].split(" ")
                num_fmt = [num.replace("i", "j").replace("+-", "-") for num in num_str]
                num_cplx = [complex(num) for num in num_fmt]
                if all(np.abs(np.array(num_cplx)) > threshold):
                    after_tags.append([complex(num) for num in num_fmt])
                l = file.readline()

    print("# before tags:", len(before_tags), "| # after tags:", len(after_tags))
    before_tags = before_tags[:len(after_tags)]  # cut off lines that didn't make it through the block when the FG was shut down
    print("First", len(before_tags), "Tags are identical:", before_tags == after_tags)
    ppstring = [str(b) + '\t' + str(a) + '\n' for a,b in zip(before_tags, after_tags)]
    ctr = 0
    for a, b in zip(before_tags, after_tags):
        if a == b:
            print("#", ctr, ":", a, "\t", b)
        else:
            print("#", ctr, ": UNEQUAL:", a, "\t", b)
        ctr += 1



