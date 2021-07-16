#pragma once
#include "Common/CommonBase.h"
#include "Common/Lockable.h"
#include "Common/Uncopyable.h"
#include <vector>
#include <thread>
#include <filesystem>


namespace fs = std::filesystem;


namespace ta
{
	class ThreadTask;
	class GameDataManager;
	class CommonSpawnDataManager;
	class CommonMoveActorSystem;
	class GameDataLoadHelper;
	class Sectors;
	class SectorZoneMappingData;
}


namespace ta
{
	extern bool RegisterThreadTask(ThreadTask* task) noexcept;
	extern void StartRegisteredThreadTasksAndWait(void) noexcept;
}


namespace ta
{
	class ThreadTaskManager : public Uncopyable, public Lockable
	{
	public:
		ThreadTaskManager(void) noexcept;
		~ThreadTaskManager(void) noexcept;

		bool registerTask(ThreadTask* task) noexcept;

		void startRegisteredTasksAndWait(void) noexcept;

	private:


	private:
		std::vector<ThreadTask*> _loadTaskQueue;
		const uint32 _maxThreadCount;
	};
}


namespace ta
{
	class ThreadTask
	{
	public:
		ThreadTask(void) noexcept;
		virtual ~ThreadTask(void) noexcept;

		virtual void processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept = 0;

	};
}


namespace ta
{
	class ThreadTaskLoadGameDataFromXml : public ThreadTask
	{
	public:
		ThreadTaskLoadGameDataFromXml(void) noexcept;
		virtual ~ThreadTaskLoadGameDataFromXml(void) noexcept;

		virtual void processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept override final;

	public:
		GameDataManager* _gameDataManager;
		fs::path _filePath;
		std::vector<std::vector<GameDataLoadHelper*>>* _gameDataLoadHelperSetGroup;
	};
}


namespace ta
{
	class ThreadTaskFinishGameDataLoading : public ThreadTask
	{
	public:
		ThreadTaskFinishGameDataLoading(void) noexcept;
		virtual ~ThreadTaskFinishGameDataLoading(void) noexcept;

		virtual void processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept override final;

	public:
		GameDataManager* _gameDataManager;
		std::vector<GameDataLoadHelper*>* _gameDataLoadHelperSet;
		GameDataType _gameDataType;
	};
}


namespace ta
{
	class ThreadTaskCheckGameDataFinally : public ThreadTask
	{
	public:
		ThreadTaskCheckGameDataFinally(void) noexcept;
		virtual ~ThreadTaskCheckGameDataFinally(void) noexcept;

		virtual void processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept override final;

	public:
		GameDataManager* _gameDataManager;
		GameDataType _gameDataType;
	};
}


namespace ta
{
	class ThreadTaskLoadSpawnDataFromXml : public ThreadTask
	{
	public:
		ThreadTaskLoadSpawnDataFromXml(void) noexcept;
		virtual ~ThreadTaskLoadSpawnDataFromXml(void) noexcept;

		virtual void processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept override final;

	public:
		CommonSpawnDataManager* _spawnDataManager;
		fs::path _filePath;
	};
}


namespace ta
{
	class ThreadTaskLoadPathPointFromXml : public ThreadTask
	{
	public:
		ThreadTaskLoadPathPointFromXml(void) noexcept;
		virtual ~ThreadTaskLoadPathPointFromXml(void) noexcept;

		virtual void processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept override final;

	public:
		CommonMoveActorSystem* _moveActorSystem;
		fs::path _filePath;
	};
}


namespace ta
{
	
	class ThreadTaskLoadSectorEventSetDataFromXml : public ThreadTask
	{
	public:
		ThreadTaskLoadSectorEventSetDataFromXml(void) noexcept;
		virtual ~ThreadTaskLoadSectorEventSetDataFromXml(void) noexcept;

		virtual void processThreadTaskInNewThread(std::vector<std::thread>& output) noexcept override final;

	public:
		Sectors* _sectors;

		fs::path _filePath;
		SectorZoneMappingData* _sectorZoneMappingData;
	};
}
