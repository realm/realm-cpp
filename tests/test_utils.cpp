#include "test_utils.hpp"

harness harness::shared = harness{};

std::vector<std::pair<std::string /* path */, fun_t>>& registered_functions()
{
    static std::vector<std::pair<std::string /* path */, fun_t>> v;
    return v;
}

void register_function(std::pair<std::string /* path */, fun_t> f) {
    registered_functions().push_back(f);
}

int main() {
    std::cout<<"Launching "<<registered_functions().size()<<" tests."<<std::endl;
    for (auto& pair : registered_functions()) {
        auto path = pair.first;
        std::filesystem::remove(std::filesystem::current_path() / std::string(path));
        std::filesystem::remove(std::filesystem::current_path() / std::string(path + ".lock"));
        std::filesystem::remove(std::filesystem::current_path() / std::string(path + ".note"));
    }
    {
        std::vector<std::pair<std::string, test::task_base>> funs;
        std::transform(registered_functions().begin(),
                       registered_functions().end(), std::back_inserter(funs),
                       [](auto &pair) {
                           return std::pair{pair.first, pair.second(pair.first)};
                       });
        while (std::transform_reduce(funs.begin(), funs.end(), true,
                                     [](bool done1, bool done2) -> bool { return done1 && done2; },
                                     [](const auto& task) -> bool { return task.second.handle.done(); }) == false) {
        };
    }

    std::cout<<harness::shared.success_count<<"/"<<harness::shared.success_count + harness::shared.fail_count<<" checks completed successfully."<<std::endl;
    return harness::shared.fail_count;
}

