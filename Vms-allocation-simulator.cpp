#include <iostream>
#include <vector>
#include <random>
#include <ctime>

using namespace std;

struct Application {
    int id;
    int cpu_need;
    int ram_need;
};

struct VM {
    int id;
    int cpu_cap, ram_cap; // total capacity of the VM
    int cpu_used = 0, ram_used = 0; // currentlu used resources
    vector<int> apps; // store app IDs

    // Check if this VM can host a given application
    bool canHost(const Application& app) {
        return (cpu_used + app.cpu_need <= cpu_cap &&
                ram_used + app.ram_need <= ram_cap);
    }
    // Assign(Host) the App to this VM
    void hostApp(const Application& app) {
        cpu_used += app.cpu_need;
        ram_used += app.ram_need;
        apps.push_back(app.id);
    }
};

class Simulation {
    int maxVMs;
    int vm_cpu, vm_ram; // capacity of each VM
    int nextAppID = 1, nextVMID = 1;

    vector<VM> vms;
    mt19937 rng; // random number generator

public:
    Simulation(int maxVMs, int vm_cpu, int vm_ram)
        : maxVMs(maxVMs), vm_cpu(vm_cpu), vm_ram(vm_ram) {
        rng.seed(time(nullptr)); // seed rng with current time
    }

    // generate a random app with random CPU and RAM needs
    Application generateApp() {
        uniform_int_distribution<int> cpu_dist(1, vm_cpu / 2);
        uniform_int_distribution<int> ram_dist(1, vm_ram / 2);
        Application a { nextAppID++, cpu_dist(rng), ram_dist(rng) };
        return a;
    }

     // Allocate an app to exsting Vms
    void allocateApp(const Application& app) {
        for (auto& vm : vms) {
            if (vm.canHost(app)) {
                vm.hostApp(app);
                cout << "App#" << app.id << " allocated to VM#" << vm.id
                     << " (CPU=" << app.cpu_need << ", RAM=" << app.ram_need << ")\n";
                return;
            }
        }

        // need a new VM
        if ((int)vms.size() < maxVMs) {
            VM newVM { nextVMID++, vm_cpu, vm_ram };
            newVM.hostApp(app);
            vms.push_back(newVM);
            cout << "App#" << app.id << " allocated to NEW VM#" << newVM.id
                 << " (CPU=" << app.cpu_need << ", RAM=" << app.ram_need << ")\n";
        } else {
            cout << "App#" << app.id << " REJECTED (no capacity)\n";
        }
    }

    // run the simulation
    void run(int ticks) {
        uniform_real_distribution<double> prob(0.0, 1.0); // probability generator
        int submitted = 0, rejected = 0; // stats counter

        for (int t = 1; t <= ticks; t++) {
            if (prob(rng) < 0.4) { // 40% chance an app arrives
                Application app = generateApp();
                submitted++;
                allocateApp(app);
                // count rejected if all Vms are full
                if ((int)vms.size() == maxVMs && !vms.back().canHost(app))
                    rejected++;
            }
        }

        // summary
        cout << "\n=== Simulation Summary ===\n";
        cout << "Ticks: " << ticks << "\n";
        cout << "VMs created: " << vms.size() << "/" << maxVMs << "\n";
        for (auto& vm : vms) {
            cout << "VM#" << vm.id << " hosted apps: ";
            for (int a : vm.apps) cout << a << " ";
            cout << "(Used CPU=" << vm.cpu_used << "/" << vm.cpu_cap
                 << ", RAM=" << vm.ram_used << "/" << vm.ram_cap << ")\n";
        }
        cout << "Submitted: " << submitted << ", Rejected: " << rejected << "\n";
    }
};

int main() {
    // parameters: max VMs, VM CPU, VM RAM
    Simulation sim(5, 10, 20);
    sim.run(20);  // run for 20 ticks
    return 0;
}
