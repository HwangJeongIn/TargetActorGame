#include "Common/ThreadTaskManager.h"
#include "Common/ScopedLock.h"
#include "Common/GameDataManager.h"
#include "Common/CommonSpawnDataManager.h"
#include "Common/CommonMoveActorSystem.h"
#include "Common/Sectors.h"


namespace ta
{
	ThreadTaskManager g_threadTaskManager;

	bool RegisterThreadTask(ThreadTask* task) noexcept
	{
		return g_threadTaskManager.registerTask(task);
	}
	
	void StartRegisteredThreadTasksAndWait(void) noexcept
	{
		g_threadTaskManager.startRegisteredTasksAndWait();
	}
}


namespace ta
{
	ThreadTaskManager::ThreadTaskManager(void) noexcept
		: _maxThreadCount(10)
	{
	}

	ThreadTaskManager::~ThreadTaskManager(void) noexcept
	{
	}

	bool ThreadTaskManager::registerTask(ThreadTask* task) noexcept
	{
		ScopedLock lock(this);
		_loadTaskQueue.push_back(task);
		return true;
	}

	void ThreadTaskManager::startRegisteredTasksAndWait(void) noexcept
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
					_loadTaskQueue[currentTaskIndex]->processThreadTaskInNewThread(loadTaskThreads);
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
					_loadTaskQueue[currentTaskIndex]->processThreadTaskInNewThread(loadTaskThreads);
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
	ThreadTask::ThreadTask(void) noexcept
	{
	}

	ThreadTask::~ThreadTask(void) noexcept
	{
	}
}


namespace ta
{
	ThreadTaskLoadGameDataFromXml::ThreadTaskLoadGameDataFromXml(void) noexcept
		: _gameDataManager(nullptr)
		, _gameDataLoadHelperSetGroup(nullptr)
	{
	}

	ThreadTaskLoadGameDataFromXml::~ThreadTaskLoadGameDataFromXml(void) noexcept
	{
	}
	
	void ThreadTaskLoadGameDataFromXml::processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept
	{
		
		output.emplace_back(std::bind(&GameDataManager::loadGameDataFromXml, _gameDataManager, _filePath, _gameDataLoadHelperSetGroup));
	}
}


namespace ta
{
	ThreadTaskFinishGameDataLoading::ThreadTaskFinishGameDataLoading(void) noexcept
		: _gameDataManager(nullptr)
		, _gameDataLoadHelperSet(nullptr)
		, _gameDataType(GameDataType::Count)
	{
	}

	ThreadTaskFinishGameDataLoading::~ThreadTaskFinishGameDataLoading(void) noexcept
	{
	}

	void ThreadTaskFinishGameDataLoading::processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept
	{
		output.emplace_back(std::bind(&GameDataManager::finishGameDataLoading, _gameDataManager, _gameDataType, *_gameDataLoadHelperSet));
	}
}


namespace ta
{
	ThreadTaskCheckGameDataFinally::ThreadTaskCheckGameDataFinally(void) noexcept
		: _gameDataManager(nullptr)
		, _gameDataType(GameDataType::Count)
	{
	}

	ThreadTaskCheckGameDataFinally::~ThreadTaskCheckGameDataFinally(void) noexcept
	{
	}

	void ThreadTaskCheckGameDataFinally::processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept
	{
		output.emplace_back(std::bind(&GameDataManager::checkGameDataFinally, _gameDataManager, _gameDataType));
	}
}


namespace ta
{
	ThreadTaskLoadSpawnDataFromXml::ThreadTaskLoadSpawnDataFromXml(void) noexcept
		: _spawnDataManager(nullptr)
	{
	}

	ThreadTaskLoadSpawnDataFromXml::~ThreadTaskLoadSpawnDataFromXml(void) noexcept
	{
	}

	void ThreadTaskLoadSpawnDataFromXml::processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept
	{
		output.emplace_back(std::bind(&CommonSpawnDataManager::loadSpawnDataFromXml, _spawnDataManager, _filePath));
	}
}


namespace ta
{
	ThreadTaskLoadPathPointFromXml::ThreadTaskLoadPathPointFromXml(void) noexcept
		: _moveActorSystem(nullptr)
	{
	}

	ThreadTaskLoadPathPointFromXml::~ThreadTaskLoadPathPointFromXml(void) noexcept
	{
	}

	void ThreadTaskLoadPathPointFromXml::processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept
	{
		output.emplace_back(std::bind(&CommonMoveActorSystem::loadPathPointPathSetFromXml, _moveActorSystem, _filePath));
	}
}


namespace ta
{
	ThreadTaskLoadSectorEventSetDataFromXml::ThreadTaskLoadSectorEventSetDataFromXml(void) noexcept
		: _sectors(nullptr)
		, _sectorZoneMappingData(nullptr)
	{
	}

	ThreadTaskLoadSectorEventSetDataFromXml::~ThreadTaskLoadSectorEventSetDataFromXml(void) noexcept
	{
	}

	void ThreadTaskLoadSectorEventSetDataFromXml::processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept
	{
		output.emplace_back(std::bind(&Sectors::loadSectorEventSetDataFromXml, _sectors, _filePath, _sectorZoneMappingData));
	}
}

