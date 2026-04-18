#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

// اسم الملف الرئيسي الذي سيحتوي على سجلات كل الملفات في نظامنا المصغر
const string StorageFileName = "MiniFileSystem.txt";

// هيكل لتمثيل "الملف" في نظامنا المصغر
struct sFileRecord
{
    string FileName;
    string FilePath;    // المسار أو الدليل (للبساطة، سنفترض أنه مسار مسطح)
    long FileSize;      // حجم الملف بالبايت
    string CreationDate;
    bool MarkedForDeletion = false;
};

// *التحويل من سطر إلى سجل ومن سجل إلى سطر*
// سنستخدم دالة بسيطة للتحويل بدلاً من الدوال المعقدة لـ SplitString
sFileRecord ConvertLineToRecord(string Line, string Separator = "#//#")
{
    sFileRecord Record;
    vector<string> vData;
    size_t pos = 0;
    string token;
    string s = Line;

    while ((pos = s.find(Separator)) != string::npos) {
        token = s.substr(0, pos);
        vData.push_back(token);
        s.erase(0, pos + Separator.length());
    }
    vData.push_back(s); // إضافة الجزء الأخير

    if (vData.size() >= 4) {
        Record.FileName = vData[0];
        Record.FilePath = vData[1];
        // تحويل السلسلة إلى رقم طويل (Long) للحجم
        try {
            Record.FileSize = stol(vData[2]);
        }
        catch (...) {
            Record.FileSize = 0;
        }
        Record.CreationDate = vData[3];
    }
    return Record;
}

string ConvertRecordToLine(sFileRecord Record, string Separator = "#//#")
{
    string stRecord = "";
    stRecord += Record.FileName + Separator;
    stRecord += Record.FilePath + Separator;
    stRecord += to_string(Record.FileSize) + Separator;
    stRecord += Record.CreationDate;
    return stRecord;
}

// *تحميل البيانات من الملف*
vector<sFileRecord> LoadRecordsFromFile(string FileName)
{
    vector<sFileRecord> vRecords;
    fstream MyFile(FileName, ios::in); // Read Mode

    if (MyFile.is_open())
    {
        string Line;
        sFileRecord Record;
        while (getline(MyFile, Line))
        {
            Record = ConvertLineToRecord(Line);
            vRecords.push_back(Record);
        }
        MyFile.close();
    }
    return vRecords;
}

// *وظائف الإدخال والإضافة*
sFileRecord ReadNewFileRecord()
{
    sFileRecord Record;
    cout << "\n--- Adding New File Record ---\n";
    cout << "Enter File Name? ";
    getline(cin >> ws, Record.FileName);
    cout << "Enter File Path (e.g. /Documents/)? ";
    getline(cin, Record.FilePath);
    cout << "Enter File Size (in bytes)? ";
    cin >> Record.FileSize;
    cout << "Enter Creation Date (YYYY-MM-DD)? ";
    cin >> ws; // مسح المخزن قبل قراءة التاريخ
    getline(cin, Record.CreationDate);
    return Record;
}

void AddDataLineToFile(string FileName, string stDataLine)
{
    fstream MyFile(FileName, ios::out | ios::app); // Append Mode
    if (MyFile.is_open())
    {
        MyFile << stDataLine << endl;
        MyFile.close();
    }
}

void AddNewFile()
{
    sFileRecord Record = ReadNewFileRecord();
    AddDataLineToFile(StorageFileName, ConvertRecordToLine(Record));
    cout << "\nFile Record [" << Record.FileName << "] Added Successfully!";
}

// *وظيفة البحث*
bool FindFileByName(string FileName, vector<sFileRecord> vRecords, sFileRecord& FoundRecord)
{
    for (sFileRecord R : vRecords)
    {
        if (R.FileName == FileName)
        {
            FoundRecord = R;
            return true;

        }
    }
    return false;
}

void ShowFindFileScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tFind File Screen";
    cout << "\n-----------------------------------\n";

    vector<sFileRecord> vRecords = LoadRecordsFromFile(StorageFileName);
    sFileRecord Record;
    string FileName = "";
    cout << "\nPlease enter File Name to search? ";
    cin >> ws;
    getline(cin, FileName);;

    if (FindFileByName(FileName, vRecords, Record))
    {
        cout << "\nFile Found:\n";
        cout << "Name: " << Record.FileName << endl;
        cout << "Path: " << Record.FilePath << endl;
        cout << "Size: " << Record.FileSize << " bytes" << endl;
        cout << "Date: " << Record.CreationDate << endl;
    }
    else
    {
        cout << "\nFile with Name [" << FileName << "] is not found in the system!";
    }
}

// *وظيفة الحذف (و إعادة كتابة الملف)*
void SaveRecordsToFile(string FileName, vector<sFileRecord> vRecords)
{
    fstream MyFile(FileName, ios::out); // Overwrite Mode
    string DataLine;
    if (MyFile.is_open())
    {
        for (sFileRecord R : vRecords)
        {
            if (R.MarkedForDeletion == false)
            {
                DataLine = ConvertRecordToLine(R);
                MyFile << DataLine << endl;
            }
        }

        MyFile.close();
    }
}

bool DeleteFileByName(string FileName, vector<sFileRecord>& vRecords)
{
    sFileRecord Record;
    char Answer = 'n';

    if (FindFileByName(FileName, vRecords, Record))
    {
        cout << "\nFile Found: " << Record.FileName << ". Are you sure you want to delete? y/n? ";
        cin >> Answer;

        if (Answer == 'y' || Answer == 'Y')
        {
            // تحديد السجل للحذف
            for (sFileRecord& R : vRecords)
            {
                if (R.FileName == FileName)
                {
                    R.MarkedForDeletion = true;
                    break;
                }
            }
            // إعادة كتابة الملف بدون السجلات المحددة للحذف
            SaveRecordsToFile(StorageFileName, vRecords);
            cout << "\nFile [" << FileName << "] Deleted Successfully.";
            return true;
        }
    }
    else
    {
        cout << "\nFile with Name (" << FileName << ") is Not Found!";
        return false;
    }
    return false;
}

void ShowDeleteFileScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tDelete File Screen";
    cout << "\n-----------------------------------\n";
    vector<sFileRecord> vRecords = LoadRecordsFromFile(StorageFileName);
    string FileName = "";
    cout << "\nPlease enter File Name to delete? ";
    cin >> ws;
    getline(cin, FileName);
    DeleteFileByName(FileName, vRecords);
}

// *وظيفة عرض جميع الملفات*
void PrintFileRecordLine(sFileRecord Record)
{
    cout << "| " << setw(20) << left << Record.FileName;
    cout << "| " << setw(30) << left << Record.FilePath;
    cout << "| " << setw(15) << left << Record.FileSize;
    cout << "| " << setw(15) << left << Record.CreationDate;
}

void ShowAllFilesScreen()
{
    vector<sFileRecord> vRecords = LoadRecordsFromFile(StorageFileName);
    cout << "\n\t\t\t\tFiles List (" << vRecords.size() << ") File(s).";
    cout << "\n_____________________________\n" << endl;
    cout << "| " << left << setw(20) << "File Name";
    cout << "| " << left << setw(30) << "File Path";
    cout << "| " << left << setw(15) << "Size (Bytes)";
    cout << "| " << left << setw(15) << "Creation Date";
    cout << "\n_____________________________\n" << endl;

    if (vRecords.size() == 0)
        cout << "\t\t\t\tNo Files Available In the System!";
    else
        for (sFileRecord Record : vRecords)
        {
            PrintFileRecordLine(Record);
            cout << endl;
        }
    cout << "\n_____________________________\n" << endl;
}

// *القائمة الرئيسية (Main Menu)*

enum enMainMenueOptions {
    eListFiles = 1, eAddNewFile = 2, eDeleteFile = 3, eFindFile = 4, eExit = 5
};

short ReadMainMenueOption()
{
    cout << "Choose what do you want to do? [1 to 5]? ";
    short Choice = 0;
    cin >> Choice;
    return Choice;
}

void GoBackToMainMenue();

void PerfromMainMenueOption(enMainMenueOptions MainMenueOption)
{
    system("cls"); // استخدام cls لمسح الشاشة
    switch (MainMenueOption)
    {
    case enMainMenueOptions::eListFiles:
        ShowAllFilesScreen();
        GoBackToMainMenue();
        break;
    case enMainMenueOptions::eAddNewFile:
        AddNewFile();
        GoBackToMainMenue();
        break;
    case enMainMenueOptions::eDeleteFile:
        ShowDeleteFileScreen();
        GoBackToMainMenue();
        break;
    case enMainMenueOptions::eFindFile:
        ShowFindFileScreen();
        GoBackToMainMenue();
        break;
    case enMainMenueOptions::eExit:
        cout << "\n-----------------------------------\n";
        cout << "\tProgram Ends :-)";
        cout << "\n-----------------------------------\n";
        break;
    }
}

void ShowMainMenue()
{
    system("cls");
    cout << "===========================================\n";
    cout << "\tMini File System Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] Show All Files List.\n";
    cout << "\t[2] Add New File.\n";
    cout << "\t[3] Delete File.\n";
    cout << "\t[4] Find File.\n";
    cout << "\t[5] Exit.\n";
    cout << "===========================================\n";
    PerfromMainMenueOption((enMainMenueOptions)ReadMainMenueOption());
}

void GoBackToMainMenue()
{
    cout << "\n\nPress any key to go back to Main Menue...";
    // استخدام cin.ignore() و cin.get() بدلاً من system("pause>0") إذا لم يكن متاحًا
    cin.ignore();
    cin.get();
    ShowMainMenue();
}

int main()
{
    ShowMainMenue();
    return 0;
}