Imports System.Drawing
Imports System.Text.RegularExpressions
Imports System.Windows.Forms
Imports System.Windows.Input
Imports Microsoft.VisualStudio.TestTools.UITest.Extension
Imports Microsoft.VisualStudio.TestTools.UITesting
Imports Microsoft.VisualStudio.TestTools.UITesting.Keyboard
Imports Microsoft.VisualStudio.TestTools.UnitTesting

<CodedUITest()>
Public Class CodedUITest1

    <TestMethod()>
    Public Sub CodedUITestMethod1()
        '            
        ' To generate code for this test, select "Generate Code for Coded UI Test" from the shortcut menu and select one of the menu items.
        '
        Me.UIMap.HelpMenuTest()
    End Sub

    <TestMethod()>
    Public Sub CodedUITestMethod2()
        '            
        ' To generate code for this test, select "Generate Code for Coded UI Test" from the shortcut menu and select one of the menu items.
        '
        Me.UIMap.InputDouble()
        Me.UIMap.AssertDoubleCheck()
        Me.UIMap.InputString()
        Me.UIMap.AssertStringCheck()
        Me.UIMap.InputFormula()
        Me.UIMap.AssertFormulaCheck()
        Me.UIMap.InputFormulaError()
        Me.UIMap.AssertFormulaErrorCheck()
        Me.UIMap.CloseSpreadsheet()

    End Sub

    <TestMethod()>
    Public Sub CodedUITestMethod3()
        '            
        ' To generate code for this test, select "Generate Code for Coded UI Test" from the shortcut menu and select one of the menu items.
        '
        Me.UIMap.MenuNewClose()

    End Sub

    <TestMethod()>
    Public Sub CodedUITestMethod4()
        '            
        ' To generate code for this test, select "Generate Code for Coded UI Test" from the shortcut menu and select one of the menu items.
        '
        Me.UIMap.undoRedoTestPart1()
        Me.UIMap.undoRedoAssertPart1()
        Me.UIMap.undoRedoTestPart2()
        Me.UIMap.undoRedoAssertPart2()
        Me.UIMap.undoRedoTestPart3()
        Me.UIMap.undoRedoAssertPart3()
        Me.UIMap.undoRedoClose()

    End Sub

    <TestMethod()>
    Public Sub CodedUITestMethod5()
        '            
        ' To generate code for this test, select "Generate Code for Coded UI Test" from the shortcut menu and select one of the menu items.
        '
        Me.UIMap.saveLoadTestPart1()
        Me.UIMap.saveLoadAssertPart1()
        Me.UIMap.saveLoadTestPart2()
        Me.UIMap.saveLoadAssertPart2()
        Me.UIMap.saveLoadTestPart3()
        Me.UIMap.saveLoadAssertPart3()
        Me.UIMap.saveLoadTestPart4()
        Me.UIMap.saveLoadClose()

    End Sub

#Region "Additional test attributes"
        '
        ' You can use the following additional attributes as you write your tests:
        '
        '' Use TestInitialize to run code before running each test
        '<TestInitialize()> Public Sub MyTestInitialize()
        '    '
        '    ' To generate code for this test, select "Generate Code for Coded UI Test" from the shortcut menu and select one of the menu items.
        '    '
        'End Sub

        '' Use TestCleanup to run code after each test has run
        '<TestCleanup()> Public Sub MyTestCleanup()
        '    '
        '    ' To generate code for this test, select "Generate Code for Coded UI Test" from the shortcut menu and select one of the menu items.
        '    '
        'End Sub

#End Region

        '''<summary>
        '''Gets or sets the test context which provides
        '''information about and functionality for the current test run.
        '''</summary>
        Public Property TestContext() As TestContext
            Get
                Return testContextInstance
            End Get
            Set(ByVal value As TestContext)
                testContextInstance = Value
            End Set
        End Property

        Private testContextInstance As TestContext

    Public ReadOnly Property UIMap As SpreadsheetUITest.UIMap
        Get
            If (Me.map Is Nothing) Then
                Me.map = New SpreadsheetUITest.UIMap()
            End If

            Return Me.map
        End Get
    End Property
    Private map As SpreadsheetUITest.UIMap
End Class
