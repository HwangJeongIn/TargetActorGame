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
	class ThreadLoadTask;
	class GameDataManager;
	class CommonSpawnDataManager;
	class CommonMoveActorSystem;
}


namespace ta
{
	extern bool RegisterThreadLoadTask(ThreadLoadTask* task) noexcept;
	extern void StartRegisteredThreadLoadTasksAndWait(void) noexcept;
}


namespace ta
{
	class ThreadLoadTaskManager : public Uncopyable, public Lockable
	{
	public:
		ThreadLoadTaskManager(void) noexcept;
		~ThreadLoadTaskManager(void) noexcept;

		bool registerTask(ThreadLoadTask* task) noexcept;

		void startRegisteredTasksAndWait(void) noexcept;

	private:


	private:
		std::vector<ThreadLoadTask*> _loadTaskQueue;
		const uint32 _maxThreadCount;
	};
}


namespace ta
{
	class ThreadLoadTask
	{
	public:
		ThreadLoadTask(void) noexcept;
		virtual ~ThreadLoadTask(void) noexcept;

		virtual void processLoadTaskInNewThread(std::vector<std::thread>& output) noexcept = 0;

	};
}


namespace ta
{
	class ThreadLoadTaskGameData : public ThreadLoadTask
	{
	public:
		ThreadLoadTaskGameData(void) noexcept;
		virtual ~ThreadLoadTaskGameData(void) noexcept;

		virtual void processLoadTaskInNewThread(std::vector<std::thread>& output) noexcept override final;

	public:
		GameDataManager* _gameDataManager;
		fs::path _filePath;
	};
}


namespace ta
{
	class ThreadLoadTaskSpawnData : public ThreadLoadTask
	{
	public:
		ThreadLoadTaskSpawnData(void) noexcept;
		virtual ~ThreadLoadTaskSpawnData(void) noexcept;

		virtual void processLoadTaskInNewThread(std::vector<std::thread>& output) noexcept override final;

	public:
		CommonSpawnDataManager* _spawnDataManager;
		fs::path _filePath;
	};
}


namespace ta
{
	class ThreadLoadTaskPathPoint : public ThreadLoadTask
	{
	public:
		ThreadLoadTaskPathPoint(void) noexcept;
		virtual ~ThreadLoadTaskPathPoint(void) noexcept;

		virtual void processLoadTaskInNewThread(std::vector<std::thread>& output) noexcept override final;

	public:
		CommonMoveActorSystem* _moveActorSystem;
		fs::path _filePath;
	};
}
