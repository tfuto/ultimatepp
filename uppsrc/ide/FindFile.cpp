#include "ide.h"

static int CharFilterFindFileMask(int c)
{
	return ToUpper(ToAscii(c));
}

class FindFileData : public Moveable<FindFileData> {
public:
	FindFileData(const String& file, const String& package);
	
	String GetFile()    const;
	String GetPackage() const;
	
private:
	String file;
	String package;
};

FindFileData::FindFileData(const String& file, const String& package)
	: file(file)
	, package(package)
{
}

String FindFileData::GetFile() const
{
	return file;
}

String FindFileData::GetPackage() const
{
	return package;
}

class FindFileWindow final : public WithFindFileLayout<TopWindow> {
private:
	struct FindFileFileDisplay : public Display {
		void Paint(Draw& w, const Rect& r, const Value& q,
				   Color ink, Color paper, dword style) const override
		{
			String txt = q;
			Image fileImage = IdeFileImage(txt, false, false);
		
			int cellHeight = r.bottom - r.top;
		
			w.DrawRect(r, paper);
			w.DrawImage(r.left, r.top + ((cellHeight - fileImage.GetHeight()) / 2), fileImage);
			w.DrawText(r.left + fileImage.GetWidth() + Zx(5), r.top +
					  ((cellHeight - StdFont().GetCy()) / 2),
					  txt, StdFont(), ink);
		}
	};

public:
	FindFileWindow(const Workspace& wspc, const String& acctualPackage,
				   const String& serachString);
	
	Vector<FindFileData> GetFindedFilesData() const;
	
private:
	void OnSearch();
	
	bool DoseFileMeetTheCriteria(
		const Package::File& file, const String& packageName, const String& query);
	bool IsActualPackage(const String& packageName);
	
private:
	const String     actualPackage;
	const Workspace& wspc;
};

FindFileWindow::FindFileWindow(const Workspace& wspc, const String& actualPackage,
							   const String& serachString)
	: wspc(wspc)
	, actualPackage(actualPackage)
{
	CtrlLayoutOKCancel(*this, "Find File");
	Sizeable().Zoomable();
	list.AddColumn("File").SetDisplay(Single<FindFileFileDisplay>());
	list.AddColumn("Package");
	list.WhenLeftDouble = Acceptor(IDOK);
	list.SetLineCy(max(Zy(16), Draw::GetStdFontCy()));
	list.EvenRowColor();
	list.MultiSelect();
	mask.NullText("Search");
	mask.SetText(serachString);
	mask.SelectAll();
	mask.SetFilter(CharFilterFindFileMask);
	mask << [=] { OnSearch(); };
	searchInCurrentPackage << [=] { OnSearch(); };
	
	OnSearch();
}

Vector<FindFileData> FindFileWindow::GetFindedFilesData() const
{
	Vector<FindFileData> data;
	
	for(int i = 0; i < list.GetCount(); i++) {
		if(list.IsSelected(i)) {
			data.Add(FindFileData(list.Get(i, 0), list.Get(i, 1)));
		}
	}
	
	return data;
}

void FindFileWindow::OnSearch()
{
	list.Clear();
	String maskValue = ~mask;
	for(int p = 0; p < wspc.GetCount(); p++) {
		String packageName = wspc[p];
		const Package& pack = wspc.GetPackage(p);
		for(const auto& file : pack.file) {
			if(DoseFileMeetTheCriteria(file, packageName, maskValue)) {
				list.Add(file, packageName);
			}
		}
	}
	if(list.GetCount() > 0)
		list.SetCursor(0);
	
	ok.Enable(list.IsCursor());
}

bool FindFileWindow::DoseFileMeetTheCriteria(const Package::File& file, const String& packageName,
                                             const String& query)
{
	if (searchInCurrentPackage && !IsActualPackage(packageName))
		return false;
	
	return !file.separator && (ToUpper(packageName).Find(query) >= 0 || ToUpper(file).Find(query) >= 0);
}

bool FindFileWindow::IsActualPackage(const String& packageName)
{
	return actualPackage.Compare(packageName) == 0;
}

void Ide::FindFileName()
{
	FindFileWindow findFileWindow(IdeWorkspace(), actualpackage, find_file_search_string);
	if(findFileWindow.Execute() == IDOK) {
		find_file_search_string = ~findFileWindow.mask;
		
		Vector<FindFileData> data = findFileWindow.GetFindedFilesData();
		for(const FindFileData& currentData : data) {
			AddHistory();
			
			String filePath = SourcePath(currentData.GetPackage(), currentData.GetFile());
			EditFile(filePath);
		}
	}
}
