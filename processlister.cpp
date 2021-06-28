#include "processlister.h"

#include <algorithm>
#ifdef Q_OS_WINDOWS
#include <libopen/processes.h>
#else
#include <plib.hh>
#endif

std::vector<ProcessLister::Process> ProcessLister::getRunningProcesses() {
	std::vector<ProcessLister::Process> result;
#ifdef Q_OS_WINDOWS
	auto processes = libopen::RunningProcesses(NULL, NULL);
	result.reserve(processes.size());
	std::transform(processes.begin(), processes.end(),
			std::back_inserter(result),
			[](const libopen::PROCESS& p){
				ProcessLister::Process process = {
					QString::fromStdString(p.exeName),
					p.Id
				};
				return process;
	});
#else
    auto processes = plib::get_all_processes();
	result.reserve(processes.size());
	std::transform(processes.begin(), processes.end(),
			std::back_inserter(result),
			[](const plib::Process& p){
				ProcessLister::Process process = {
					QString::fromStdString(p.name_get()),
					static_cast<int>(p.stat_get().pid)
				};
				return process;
	});
#endif
	return result;
}
