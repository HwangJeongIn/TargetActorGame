#include "Common/ThreadLoadTaskManager.h"
#include "Common/ScopedLock.h"
#include "Common/GameDataManager.h"
#include "Common/CommonSpawnDataManager.h"

namespace ta
{
	ThreadLoadTaskManager g_threadTaskManager;

	bool RegisterThreadLoadTask(ThreadLoadTask* task) noexcept
	{
		return g_threadTaskManager.registerTask(task);
	}
	
	void StartRegisteredThreadLoadTasksAndWait(void) noexcept
	{
		g_threadTaskManager.startRegisteredTasksAndWait();
	}
}


namespace ta
{
	ThreadLoadTaskManager::ThreadLoadTaskManager(void) noexcept
		: _maxThreadCount(10)
	{
	}

	ThreadLoadTaskManager::~ThreadLoadTaskManager(void) noexcept
	{
	}

	bool ThreadLoadTaskManager::registerTask(ThreadLoadTask* task) noexcept
	{
		ScopedLock lock(this);
		_loadTaskQueue.push_back(task);
		return true;
	}

	void ThreadLoadTaskManager::startRegisteredTasksAndWait(void) noexcept
	{
		ScopedLock lock(this);

		const uint32 count = _loadTaskQueue.size();

		std::vector<std::thread> loadTaskThreads;
		int32 currentTaskIndex = count - 1;
		while (0 <= currentTaskIndex) // 한바퀴돌고 다시처리
		{
			if (_maxThreadCount < (uint32)(currentTaskIndex + 1))
			{
				for (uint32 currentCount = 0; currentCount < _maxThreadCount; ++currentCount)
				{
					_loadTaskQueue[currentTaskIndex]->processLoadTaskInNewThread(loadTaskThreads);
					--currentTaskIndex;
				}

				TA_ASSERT_DEV(loadTaskThreads.size() == _maxThreadCount, "코드버그");
				const uint32 threadCount = loadTaskThreads.size();
				for (uint32 threadIndex = 0; threadIndex < threadCount; ++threadIndex)
				{
					loadTaskThreads[threadIndex].join();
				}

				loadTaskThreads.clear();
				loadTaskThreads.reserve(_maxThreadCount);
			}
			else // 이번것만 마치고 끝내면된다.
			{
				while (0 <= currentTaskIndex)
				{
					_loadTaskQueue[currentTaskIndex]->processLoadTaskInNewThread(loadTaskThreads);
					--currentTaskIndex;
				}

				const uint32 threadCount = loadTaskThreads.size();
				for (uint32 threadIndex = 0; threadIndex < threadCount; ++threadIndex)
				{
					loadTaskThreads[threadIndex].join();
				}

				loadTaskThreads.clear();
				break;
			}

		}

		// delete all finished task
		{
			for (uint32 index = 0; index < count; ++index)
			{
				delete _loadTaskQueue[index];
			}

			_loadTaskQueue.clear();
		}
	}
}


namespace ta
{
	ThreadLoadTask::ThreadLoadTask(void) noexcept
	{
	}

	ThreadLoadTask::~ThreadLoadTask(void) noexcept
	{
	}
}


namespace ta
{
	ThreadLoadTaskGameData::ThreadLoadTaskGameData(void) noexcept
	{
	}

	ThreadLoadTaskGameData::~ThreadLoadTaskGameData(void) noexcept
	{
	}
	
	void ThreadLoadTaskGameData::processLoadTaskInNewThread(std::vector<std::thread>& output) noexcept
	{
		output.emplace_back(GameDataManager::loadGameDataFromXml, _gameDataManager, _filePath);
	}
}


namespace ta
{
	ThreadLoadTaskSpawnData::ThreadLoadTaskSpawnData(void) noexcept
	{
	}

	ThreadLoadTaskSpawnData::~ThreadLoadTaskSpawnData(void) noexcept
	{
	}

	void ThreadLoadTaskSpawnData::processLoadTaskInNewThread(std::vector<std::thread>& output) noexcept
	{
		output.emplace_back(CommonSpawnDataManager::loadSpawnDataFromXml, _spawnDataManager, _filePath);
	}
}
