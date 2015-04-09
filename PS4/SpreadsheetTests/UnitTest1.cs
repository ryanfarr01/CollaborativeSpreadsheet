using System;
using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SS;
using SpreadsheetUtilities;
using System.Text.RegularExpressions;

namespace spreadsheetTests
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void Constructor0()
        {
            spreadsheet spreadsheet = new spreadsheet();
            List<string> list = new List<string>(spreadsheet.GetNamesOfAllNonemptyCells());
            Assert.AreEqual(0, list.Count); // the spreadsheet has only empty cells

        }

        [TestMethod]
        public void Constructor3()
        {
            spreadsheet spreadsheet = new spreadsheet(isValidCellName, s => s.ToUpper(), "Version");
            List<string> list = new List<string>(spreadsheet.GetNamesOfAllNonemptyCells());
            Assert.AreEqual(0, list.Count); // the spreadsheet has only empty cells
            Assert.AreEqual("Version", spreadsheet.Version);
        }

        [TestMethod]
        public void Constructor4a() // successful load
        {
            spreadsheet spreadsheet = new spreadsheet();

            spreadsheet.SetContentsOfCell("A1", "this is a string");
            spreadsheet.SetContentsOfCell("B1", "2.6");
            spreadsheet.SetContentsOfCell("C1", "=1.5+10");

            spreadsheet.Save("testxml.xml");

            spreadsheet = new spreadsheet("testxml.xml",s=>true,s=>s,"default");

            Assert.AreEqual("this is a string", spreadsheet.GetCellContents("A1"));
            Assert.AreEqual(2.6, spreadsheet.GetCellContents("B1"));
            Assert.AreEqual(new Formula("1.5+10"), spreadsheet.GetCellContents("C1"));
        }

        [TestMethod]
        [ExpectedException(typeof(SpreadsheetReadWriteException))]
        public void Constructor4b() // failed load - incorrect version:
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", "this is a string");
            spreadsheet.Save("testxml.xml");
            spreadsheet = new spreadsheet("testxml.xml", s => true, s => s, "wrong version");
        }

        [TestMethod]
        [ExpectedException(typeof(SpreadsheetReadWriteException))]
        public void Constructor4c() // failed load - bad cell name:
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", "this is a string");
            spreadsheet.Save("testxml.xml");
            spreadsheet = new spreadsheet("testxml.xml", s => false, s => s, "default");
        }

        [TestMethod]
        [ExpectedException(typeof(SpreadsheetReadWriteException))]
        public void Constructor4d() // failed load - Circular exception:
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", "B1+5");
            spreadsheet.SetContentsOfCell("B1", "a1+5");
            spreadsheet.SetContentsOfCell("a1", "=A1+5");
            spreadsheet.Save("testxml.xml");
            spreadsheet = new spreadsheet("testxml.xml", s => true, s => s.ToUpper(), "default");
        }

        [TestMethod]
        [ExpectedException(typeof(SpreadsheetReadWriteException))]
        public void Constructor4e() // failed load - Bad file name:
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", "10");
            spreadsheet.SetContentsOfCell("B1", "=A1+5");
            spreadsheet.Save("testxml.xml");
            spreadsheet = new spreadsheet("testxmlbad.xml", s => true, s => s.ToUpper(), "default");
        }

        [TestMethod]
        [ExpectedException(typeof(SpreadsheetReadWriteException))]
        public void Constructor4f() // failed load - Bad formula:
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", "=BB11+5");
            spreadsheet.Save("testxml.xml");
            spreadsheet = new spreadsheet("testxml.xml", isValidCellName, s => s.ToUpper(), "default");
        }

        [TestMethod]
        public void SaveTest()
        {
            spreadsheet spreadsheet = new spreadsheet();

            spreadsheet.SetContentsOfCell("A1", "this is a string");
            spreadsheet.SetContentsOfCell("B1", "2.6");
            spreadsheet.SetContentsOfCell("C1", "=1.5+10");

            spreadsheet.Save("testxml.xml");
        }

        [TestMethod]
        public void GetVersionTest()
        {
            spreadsheet spreadsheet = new spreadsheet();

            spreadsheet.SetContentsOfCell("A1", "this is a string");
            spreadsheet.SetContentsOfCell("B1", "2.6");
            spreadsheet.SetContentsOfCell("C1", "=1.5+10");

            spreadsheet.Save("testxml.xml");

            Assert.AreEqual("default",spreadsheet.GetSavedVersion("testxml.xml"));

            spreadsheet = new spreadsheet(s => true, s => s, "Version 1");

            spreadsheet.SetContentsOfCell("A1", "this is a string");
            spreadsheet.SetContentsOfCell("B1", "2.6");
            spreadsheet.SetContentsOfCell("C1", "=1.5+10");

            spreadsheet.Save("testxml.xml");

            Assert.AreEqual("Version 1", spreadsheet.GetSavedVersion("testxml.xml"));
            
        }
        

        // Test than an InvalidNameException is thrown when a null name is used and 
        // an invalid name is used when setting the contents of a cell for each of
        // the cell setting methods
        [TestMethod]
        [ExpectedException(typeof(InvalidNameException))]
        public void SetContentsOfCellNameNull1()
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell(null, "1");
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidNameException))]
        public void SetContentsOfCellNameInvalid1()
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("1a", "1");
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentNullException))]
        public void SetContentsOfCellContentNull()
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", null);
        }
        
        // Test set contents to double
        [TestMethod]
        public void SetCellNumber()
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", "1");
            spreadsheet.SetContentsOfCell("B1", "0");
            spreadsheet.SetContentsOfCell("C2", "-10");

            List<string> list = new List<string>(spreadsheet.GetNamesOfAllNonemptyCells());
            Assert.AreEqual(3, list.Count);
            Assert.IsTrue(list.Contains("A1"));
            Assert.IsTrue(list.Contains("B1"));
            Assert.IsTrue(list.Contains("C2"));

            Assert.AreEqual(1, (double)spreadsheet.GetCellContents("A1"));
        }
        

        // Test set contents to string
        [TestMethod]
        public void SetCellString()
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", "Test 1");
            spreadsheet.SetContentsOfCell("B1", "Test 2");
            spreadsheet.SetContentsOfCell("C2", "Test 3");

            List<string> list = new List<string>(spreadsheet.GetNamesOfAllNonemptyCells());
            Assert.AreEqual(3, list.Count);
            Assert.IsTrue(list.Contains("A1"));
            Assert.IsTrue(list.Contains("B1"));
            Assert.IsTrue(list.Contains("C2"));

            Assert.AreEqual("Test 1", (string)spreadsheet.GetCellContents("A1"));
            Assert.AreEqual("Test 2", (string)spreadsheet.GetCellContents("B1"));
            Assert.AreEqual("Test 3", (string)spreadsheet.GetCellContents("C2"));

            spreadsheet.SetContentsOfCell("A1", "");

            list = new List<string>(spreadsheet.GetNamesOfAllNonemptyCells());
            Assert.AreEqual(2, list.Count);
        }

        // Test set contents to formula
        [TestMethod]
        public void SetCellFormula()
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", "=1+1");
            spreadsheet.SetContentsOfCell("B1", "=A1+1");
            spreadsheet.SetContentsOfCell("C2", "=A1+B1");

            List<string> list = new List<string>(spreadsheet.GetNamesOfAllNonemptyCells());
            Assert.AreEqual(3, list.Count);
            Assert.IsTrue(list.Contains("A1"));
            Assert.IsTrue(list.Contains("B1"));
            Assert.IsTrue(list.Contains("C2"));

            // test for chain formula

            Assert.AreEqual(new Formula("1+1"), (Formula)spreadsheet.GetCellContents("A1"));
            Assert.AreEqual(new Formula("A1+1"), (Formula)spreadsheet.GetCellContents("B1"));
            Assert.AreEqual(new Formula("A1+B1"), (Formula)spreadsheet.GetCellContents("C2"));

            Assert.AreEqual(2.0, spreadsheet.GetCellValue("A1"));
            Assert.AreEqual(3.0, spreadsheet.GetCellValue("B1"));
            Assert.AreEqual(5.0, spreadsheet.GetCellValue("C2"));

            // test for chain formula with a mid step changed

            spreadsheet.SetContentsOfCell("B1", "=10");

            Assert.AreEqual(10.0, spreadsheet.GetCellValue("B1"));
            Assert.AreEqual(12.0, spreadsheet.GetCellValue("C2"));

            list = new List<string>(spreadsheet.SetContentsOfCell("B1", "=10"));
            Assert.AreEqual(2, list.Count);
            Assert.IsTrue(list.Contains("C2"));
            Assert.IsTrue(list.Contains("B1"));
        }

        [TestMethod]
        [ExpectedException(typeof(CircularException))]
        public void SetCellFormula2()
        {
            spreadsheet spreadsheet = new spreadsheet();
            spreadsheet.SetContentsOfCell("A1", "=B1");
            spreadsheet.SetContentsOfCell("B1", "=C2");
            spreadsheet.SetContentsOfCell("C2", "=A1");
        }
        
        // Test get cell contents - this method has largely been tested above. These tests will ensure the error 
        // handling is done corrctly and test getting an empty cell contents
        [TestMethod]
        [ExpectedException(typeof(InvalidNameException))]
        public void GetCellContents()
        {
            spreadsheet spreadsheet = new spreadsheet();
            Assert.AreEqual("", spreadsheet.GetCellContents("A1"));
            spreadsheet.GetCellContents(null);
        }

        public Boolean isValidCellName(string s){
            Regex nameChecker = new Regex("A[0-9]+", 0);
            return nameChecker.IsMatch(s); 
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidNameException))]
        public void isValidCheck()
        {
            spreadsheet spreadsheet = new spreadsheet(isValidCellName, s => s.ToUpper(), "Version");
            spreadsheet.SetContentsOfCell("ABC123", "1");
        }

        [TestMethod]
        public void TestMethod1()
        {
        }
         
    }
}
