import time
import matplotlib.pyplot as plt
import subprocess
import random
import numpy

def call():
    start = time.time() * 1000
    gcc_call = subprocess.run(["bash", "./call.sh"],
                              stdout=subprocess.PIPE)
    end = time.time() * 1000
    mem = int(gcc_call.stdout)
    return end - start, mem


def creation(method, len_of_list, suffix=""):
    list_type = "type_list<>"
    type_names = ["int", "float", "long long"]
    for i in range(0, len_of_list):
        type_id = random.randint(0, 2)
        list_type = f"{method}<{list_type}, {type_names[type_id]}>"
    return f"using {method}_creation{suffix} = {list_type};"


def generate(f, len_of_list):
    f.write(f"inline constexpr int N = {len_of_list};\n")
    f.write(f"inline constexpr int S = 5;\n")
    f.write(f"inline constexpr int M = {len_of_list / 2};\n")
    f.write(f"inline constexpr int E = {len_of_list - 1};\n")
    f.write(f"using target = type_list<")
    for i in range(0, int(len_of_list / 2)):
        f.write("int, ")
    f.write("float, ")
    for i in range(int(len_of_list / 2) + 1, len_of_list - 2):
        f.write("int, ")
    f.write("double, int>;\n")
    f.write(creation("type_list_push_back", len_of_list) + "\n")
    f.write(creation("type_list_push_front", len_of_list) + "\n")
    f.write(creation("type_list_push_back", len_of_list, suffix="1"))
    f.write(creation("type_list_push_back", len_of_list, suffix="2"))
    f.write(creation("type_list_push_back", len_of_list, suffix="3"))


def compare():
    x = range(10, 5011, 100)
    variadic = []
    common = []
    recursive = []
    variadic_mem = []
    common_mem = []
    recursive_mem = []
    for len_of_list in x:
        with open("target.hpp", "w") as f:
            f.write("#include \"Tlist_variadic.hpp\"\n")
            generate(f, len_of_list)
        t, m = call()
        print(f"variadic: {len_of_list} -> {t}, {m}")
        variadic.append(t)
        variadic_mem.append(m)

        # with open("target.hpp", "w") as f:
        #     f.write("#include \"Tlist_common.hpp\"\n")
        #     f.write("using namespace Compiler;\n")
        #     generate(f, len_of_list)
        # t, m = call()
        # print(f"common: {len_of_list} -> {t}, {m}")
        # common.append(t)
        # common_mem.append(m)

        # with open("target.hpp", "w") as f:
        #     f.write("#include \"Tlist_recursive.hpp\"\n")
        #     generate(f, len_of_list)
        # t, m = call()
        # print(f"recursive: {len_of_list} -> {t}, {m}")
        # recursive.append(t)
        # recursive_mem.append(m)
    # time/mem
    time_func = numpy.poly1d(numpy.polyfit(x, variadic, 10))
    mem_func = numpy.poly1d(numpy.polyfit(x, variadic_mem, 10))
    print(time_func)
    print(mem_func)
    fig, ax1 = plt.subplots()

    ax2 = ax1.twinx()
    time_line, = ax1.plot(x, variadic, 'g', label="Time")
    mem_line, = ax2.plot(x, variadic_mem, 'b--', label="Memory")
    # ax1.plot(x, time_func(x), 'o')
    # ax2.plot(x, mem_func(x), 'y')
    ax1.set_xlabel("length of type_list")
    ax1.set_ylabel('Time Usage/ms', color='g')
    ax2.set_ylabel('Memory Usage/KB', color='b')
    ax1.legend([time_line, mem_line], ['Time', 'Memory'] ,loc="upper left")
    plt.show()
    # plt.figure(1)
    # plt.plot(x, variadic, color='g', label="variadic")
    # # plt.plot(x, common, color='b', label="common")
    # # plt.plot(x, recursive, color='o', label="recursive")
    # plt.xlabel("length of type_list")
    # plt.ylabel("time")
    # plt.title("Time Benchmark")
    # plt.legend(loc="upper left")
    # plt.figure(2)
    # plt.plot(x, variadic_mem, color='g', label="variadic")
    # # plt.plot(x, common_mem, color='b', label="common")
    # # plt.plot(x, recursive_mem, color='b', label="recursive")
    # plt.xlabel("length of type_list")
    # plt.ylabel("memory usage(kb)")
    # plt.title("Memory Usage")
    # plt.legend(loc="upper left")
    # plt.show()


def generate_set(f, max_len, small, middle, large, name='t'):
    type_list = []
    for i in range(0, max_len):
        typename = f"{name}{i}"
        type_list.append(typename)
        f.write(f"struct {typename}{{}};\n")

    def generate_type(l):
        types_of_list = random.sample(type_list, l)
        list_str = "type_list<"
        for t in types_of_list:
            list_str += f"{t},"
        list_str += "int>"
        return list_str

    s1 = generate_type(small)
    s2 = generate_type(small)
    m1 = generate_type(middle)
    m2 = generate_type(middle)
    l1 = generate_type(large)
    l2 = generate_type(large)
    return s1, s2, m1, m2, l1, l2


def first_n():
    x = range(10, 5000)
    cond = []
    cond_mem = []
    spec = []
    spec_mem = []
    for len_of_list in x:
        # with open("target.hpp", "w") as f:
        #     f.write("#include \"Tlist_variadic.hpp\"\n")
        #     f.write(f"inline constexpr int N = {len_of_list};\n")
        #     f.write(f"inline constexpr int S = 5;\n")
        #     f.write(f"inline constexpr int M = {len_of_list / 2};\n")
        #     f.write(f"inline constexpr int E = {len_of_list - 1};\n")
        #     f.write(f"using target = type_list<")
        #     for i in range(0, int(len_of_list / 2)):
        #         f.write("int, ")
        #     f.write("float, ")
        #     for i in range(int(len_of_list / 2) + 1, len_of_list - 2):
        #         f.write("int, ")
        #     f.write("double, int>;\n")
        #     f.write("template <typename T>struct id [using type = T;};\n")
        #     f.write("static_assert(std::is_same_v<type_list_for_each<target, id>, target>);")
        # t, m = call()
        # print(f"conditional_t: {len_of_list} -> {t}, {m}")
        # cond.append(t)
        # cond_mem.append(m)

        with open("target.hpp", "w") as f:
            f.write("#include \"Tlist_variadic.hpp\"\n")
            f.write(f"inline constexpr int N = {len_of_list};\n")
            f.write(f"inline constexpr int S = 5;\n")
            f.write(f"inline constexpr int M = {len_of_list / 2};\n")
            f.write(f"inline constexpr int E = {len_of_list - 1};\n")
            f.write(f"using target = type_list<")
            for i in range(0, int(len_of_list / 2)):
                f.write("int, ")
            f.write("float, ")
            for i in range(int(len_of_list / 2) + 1, len_of_list - 2):
                f.write("int, ")
            f.write("double, int>;\n")
            f.write("template <typename T>struct id [using type = T;};\n")
            f.write("static_assert(std::is_same_v<impl::type_list_for_each<target, id>, target>);")
        t, m = call()
        print(f"specialization: {len_of_list} -> {t}, {m}")
        spec.append(t)
        spec_mem.append(m)
    # time/mem
    fig, ax1 = plt.subplots()

    ax2 = ax1.twinx()
    ax1.plot(x, spec, 'g-', label="Time")
    ax2.plot(x, spec_mem, 'b-', label="Memory")
    ax1.set_xlabel("length of type_list")
    ax1.set_ylabel('Time Usage', color='g')
    ax2.set_ylabel('Memory Usage', color='b')

    # plt.figure(1)
    # plt.plot(x, cond, color='g', label="conditional_t")
    # plt.plot(x, spec, color='b', label="specialization")
    # plt.xlabel("length of type_list")
    # plt.ylabel("time")
    # plt.title("Time Benchmark")
    # plt.figure(2)
    # plt.plot(x, cond_mem, color='g', label="conditional_t")
    # plt.plot(x, spec_mem, color='b', label="specialization")
    # plt.xlabel("length of type_list")
    # plt.ylabel("memory usage(kb)")
    # plt.title("Memory Usage")
    # plt.show()


compare()
