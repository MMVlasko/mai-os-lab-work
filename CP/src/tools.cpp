#include <tools.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_set>
#include <nlohmann/json.hpp>

bool IsCyclicUtil(const int node, std::unordered_map<int, std::vector<int>>& graph,
        std::unordered_map<int, bool>& visited, std::unordered_map<int, bool>& recStack) {
    if (!visited[node]) {
        visited[node] = true;
        recStack[node] = true;

        for (const auto& neighbor : graph[node]) {
            if (!visited[neighbor] && IsCyclicUtil(neighbor, graph, visited, recStack) || recStack[neighbor])
                return true;
        }
    }

    recStack[node] = false;
    return false;
}

bool IsCyclic(std::unordered_map<int, std::vector<int>>& graph) {
    std::unordered_map<int, bool> visited, recStack;

    for (const auto& [fst, _] : graph) {
        if (IsCyclicUtil(fst, graph, visited, recStack))
            return true;
    }

    return false;
}

bool ValidateDAG(std::unordered_map<int, std::vector<int>>& graph) {
    std::unordered_set<int> ids;

    for (auto& [fst, _] : graph) {
        ids.insert(fst);
    }

    for (auto& [_, snd] : graph) {
        for (auto id : snd) {
            if (ids.find(id) == ids.end()) {
                std::cerr << "Error: DAG dependencies contains undefined ids" << std::endl;
                return false;
            }
        }
    }

    std::vector<int> startJobs, endJobs;

    if (IsCyclic(graph)) {
        std::cerr << "Error: DAG contains cycles" << std::endl;
        return false;
    }

    for (const auto&[fst, snd] : graph) {
        auto end = true;
        for (const auto&[fst, snd] : graph) {
            if (std::find(snd.begin(), snd.end(), fst) != snd.end()) {
                end = false;
                break;
            }
        }
        if (end) {
            endJobs.push_back(fst);
        }
    }

    if (endJobs.empty()) {
        std::cerr << "Error: No end jobs found" << std::endl;
        return false;
    }

    for (const auto& [job, deps] : graph) {
        if (deps.empty()) {
            startJobs.push_back(job);
        }
    }

    if (startJobs.empty()) {
        std::cerr << "Error: No start jobs found" << std::endl;
        return false;
    }

    std::unordered_set<int> visited;
    std::queue<int> q;

    if (!endJobs.empty()) {
        q.push(endJobs[0]);
        visited.insert(endJobs[0]);
    }

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (const auto& neighbor : graph[current]) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }

    if (visited.size() != graph.size()) {
        std::cerr << "Error: DAG is not a single connected component" << std::endl;
        return false;
    }

    return true;
}

void ExecuteJob(const std::string& jobName, sem_t* semaphore, std::atomic<bool>& errorFlag, const int execTime = 2) {
    if (errorFlag) {
        pthread_cond_broadcast(&queueCV);
        return;
    }

    if (semaphore) {
        sem_wait(semaphore);
    }

    pthread_mutex_lock(&stdoutMutex);

    std::cout << "Starting job: " << jobName << std::endl;

    pthread_mutex_unlock(&stdoutMutex);

    std::this_thread::sleep_for(std::chrono::seconds(execTime));

    pthread_mutex_lock(&stdoutMutex);

    if (mustBreak == jobName) {
        std::cerr << "Job failed: " << jobName << std::endl;
        errorFlag = true;
    } else {
        std::cout << "Job completed: " << jobName << std::endl;
    }

    pthread_mutex_unlock(&stdoutMutex);

    if (semaphore) {
        sem_post(semaphore);
    }
}

void *ThreadProcess(void*) {
    while (true) {
        int currentJob = -1;

        pthread_mutex_lock(&queueMutex);

        while (readyJobs.empty() && !done && !errorFlag) {
            pthread_cond_wait(&queueCV, &queueMutex);
        }

        if (done || errorFlag) {
            pthread_mutex_unlock(&queueMutex);
            break;
        }

        if (!readyJobs.empty()) {
            currentJob = readyJobs.front();
            readyJobs.pop();
            ++activeJobs;
        }

        pthread_mutex_unlock(&queueMutex);

        if (currentJob != -1) {
            sem_t *sem = nullptr;

            if (!jobs[currentJob].semaphoreName.empty()) {
                sem = &semaphores[jobs[currentJob].semaphoreName];
            }
            ExecuteJob(jobs[currentJob].name, sem, errorFlag, jobs[currentJob].time);

            pthread_mutex_lock(&queueMutex);

            --activeJobs;

            for (const auto &[job, deps]: graph) {
                if (auto it = std::find(deps.begin(), deps.end(), currentJob);
                        it != deps.end()) {
                    inDegree[job]--;

                    if (inDegree[job] == 0) {
                        readyJobs.push(job);
                        pthread_cond_signal(&queueCV);
                    }
                }
            }

            if (activeJobs == 0 && readyJobs.empty()) {
                done = true;
                pthread_cond_broadcast(&queueCV);
            }

            pthread_mutex_unlock(&queueMutex);
        }
    }

    return nullptr;
}