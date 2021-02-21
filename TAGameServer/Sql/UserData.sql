USE TAGameServer
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================

IF OBJECT_ID (N'GetErrorData', N'P') IS NOT NULL -- P는 저장프로시저
	DROP PROCEDURE GetErrorData
GO

CREATE PROCEDURE GetErrorData 
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	SELECT
		ERROR_NUMBER() AS ErrorNumber,
		ERROR_SEVERITY() AS ErrorSeverity,
		ERROR_STATE() AS ErrorState,
		ERROR_PROCEDURE() AS ErrorProcedure,
		ERROR_LINE() AS ErrorLine,
		ERROR_MESSAGE() AS ErrorMessage
END
GO


-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	SP예시 CRUD중 R는 다음과 같이 작성
-- =============================================


IF OBJECT_ID (N'GetAllUserData', N'P') IS NOT NULL
	DROP PROCEDURE GetAllUserData
GO

CREATE PROCEDURE GetAllUserData
(
	@ErrorCode INT OUTPUT
)

AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	-- don't print row count
	SET NOCOUNT ON;

	SET @ErrorCode = 0

 	BEGIN TRAN

		Select UserId, UserName, UserLevel From UserData --WHERE UserId = 10

 		COMMIT TRAN


 -- BEGIN TRY
 -- 	-- validation check
 -- 	BEGIN TRAN
 -- 	-- do something
 -- 		COMMIT TRAN
 -- END TRY
 -- BEGIN CATCH 
 -- 	-- sp error
 -- 	SET @ErrorCode = -1
 -- 	EXEC GetErrorData
 -- 
 -- 	-- rollback when transaction exist
 -- 	IF (0 < @@TRANCOUNT)
 -- 		BEGIN
 -- 			ROLLBACK TRAN
 -- 			RETURN
 -- 		END 
 -- END CATCH

END
GO


-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	SP예시 CRUD중 CUD는 다음과 같이 작성
-- =============================================


IF OBJECT_ID (N'TestSp', N'P') IS NOT NULL
	DROP PROCEDURE TestSp
GO

CREATE PROCEDURE TestSp
(
-- Add the parameters for the stored procedure here
	-- @user_id int,
	-- @user_password int
	@ErrorCode INT OUTPUT
)

AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	-- don't print row count
	SET NOCOUNT ON;

	SET @ErrorCode = 0

BEGIN TRY
	
	-- validation check

	BEGIN TRAN
		-- SELECT 1/0
		UPDATE UserData SET UserName = 'test_name' WHERE UserId = 10
		
		IF (0 = @@RowCount) -- custom error
			BEGIN
				SET @ErrorCode = 1
				--PRINT N'ErrorCode : ' + CAST(@ErrorCode AS NCHAR(3))
				ROLLBACK TRAN
				RETURN 
			END

		COMMIT TRAN

END TRY
BEGIN CATCH 

	PRINT N'sp error'

	-- sp error
	SET @ErrorCode = -1
	EXEC GetErrorData

	-- rollback when transaction exist
	IF (0 < @@TRANCOUNT)
		BEGIN
			PRINT N'rollback in catch block'
			ROLLBACK TRAN
			RETURN
		END 

END CATCH
END
GO


DECLARE @LocalVar INT

EXEC TestSp @LocalVar OUTPUT
PRINT N'ErrorCodeOutput : ' + CAST(@LocalVar AS NCHAR(3))

-- EXEC @rtn = usp_UpdatePrice -0.1, 'CE', @cnt OUTPUT
-- SELECT @cnt AS 업데이트, @rtn AS 오류

EXEC GetAllUserData  @LocalVar OUTPUT
PRINT N'ErrorCodeOutput : ' + CAST(@LocalVar AS NCHAR(3))

GO


