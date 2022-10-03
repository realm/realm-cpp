#include "test_utils.hpp"
#include "admin_utils.hpp"
#include "test_objects.hpp"
harness harness::shared = harness{};

std::vector<std::pair<std::string /* path */, fun_t>>& registered_functions()
{
    static std::vector<std::pair<std::string /* path */, fun_t>> v;
    return v;
}

void register_function(const std::pair<std::string /* path */, fun_t>& f) {
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
        for (auto& f : registered_functions()) {
            f.second(f.first);
        }
    }
    std::cout<<harness::shared.success_count<<"/"<<harness::shared.success_count + harness::shared.fail_count<<" checks completed successfully."<<std::endl;
    return harness::shared.fail_count;
}

