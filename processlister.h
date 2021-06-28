#ifndef PROCESSLISTER_H
#define PROCESSLISTER_H

#include <vector>
#include <QString>

class ProcessLister {
public:
	struct Process {
		QString name;
		int pid;
	};
	static std::vector<Process> getRunningProcesses();
};

#endif // PROCESSLISTER_H
