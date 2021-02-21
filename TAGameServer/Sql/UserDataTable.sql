-- N : 유니코드
-- U : 사용자 지정테이블
-- VAR : 가변길이
-- GO : 일괄처리 단위		-- 이전에 쓰인 GO 다음부터 현재 GO앞까지 실행한다.
						-- 이전에 선언한 로컬 변수 뒤에 GO가 들어오면 리셋된다 주의가 필요하다.

USE TAGameServer
GO

-- =========================================
-- Create UserDataTable
-- =========================================

IF OBJECT_ID(N'UserData', N'U') IS NOT NULL
  DROP TABLE UserData
GO

CREATE TABLE UserData(
	UserId					INT				NOT NULL,
	UserName				NVARCHAR(20)	NOT NULL,
	UserLevel				SMALLINT		NOT NULL	DEFAULT 0,
	FirstLoginTime			DATETIME		NOT NULL	DEFAULT GETDATE(),

	-- PRIMARY KEY(UserId)
	-- PRIMARY KEY CLUSTERED(UserId, UserName)
	CONSTRAINT PK_UserData_UserId PRIMARY KEY(UserId) -- 정확히 정의해서 사용하자
	-- CONSTRAINT PK_UserData_UserId_UserName PRIMARY KEY CLUSTERED(UserId, UserName)
	-- FOREIGN KEY temp_id REFERENCES TempData (temp_id)
)
GO
