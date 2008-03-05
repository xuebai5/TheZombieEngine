#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguifiledialog_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguifiledialog.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"
#include "gui/nguidirlister.h"
#include "gui/nguitextbutton.h"
#include "gui/nguitextentry.h"
#include "gui/nguiskin.h"
#include "gui/nguimessagebox.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nGuiFileDialog, "nguiclientwindow");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nguifiledialog

    @cppclass
    nGuiFileDialog
    
    @superclass
    nguiclientwindow
    
    @classinfo
    Docs needed.
*/

//------------------------------------------------------------------------------
/**
*/
nGuiFileDialog::nGuiFileDialog() :
    dirPath("home:"),
    saveMode(false),
    stripExtension(false),
    pattern("*")
{
    this->SetText(Ok, "Ok");
    this->SetText(Cancel, "Cancel");
    this->SetText(OverwriteMessage, "Overwrite existing file?");
    this->SetText(OverwriteOk, "Ok");
    this->SetText(OverwriteCancel, "Cancel");
    this->entryTextColor.set(0.0f, 0.0f, 0.0f, 1.0f);
    this->dirlisterTextColor.set(0.0f, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
/**
*/
nGuiFileDialog::~nGuiFileDialog()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool nGuiFileDialog::Render()
{
    if (this->refFolderTextEntry.isvalid()) {
        this->refFolderTextEntry.get()->SetActive(false);
    }
    
    return nGuiClientWindow::Render();     // call parent
}

//------------------------------------------------------------------------------
/**
*/
void  
nGuiFileDialog::OnShow()
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    // call parent class
    nGuiClientWindow::OnShow();

    // set the window title
    if (0 == this->GetTitle())
    {
        if (this->GetSaveMode())
        {
            this->SetTitle("Save File");
        }
        else
        {
            this->SetTitle("Load File");
        }
    }

    // get client area layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");
    vector2 textSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("textentry_n");

    // create Cancel button
    nGuiTextButton* cancelButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "CancelButton");
    n_assert(cancelButton);
    cancelButton->SetText(this->GetText(Cancel));
    cancelButton->SetFont("GuiSmall");
    cancelButton->SetAlignment(nGuiTextButton::Center);
    cancelButton->SetDefaultBrush("button_n");
    cancelButton->SetPressedBrush("button_p");
    cancelButton->SetHighlightBrush("button_h");
    cancelButton->SetMinSize(buttonSize);
    cancelButton->SetMaxSize(buttonSize);
    cancelButton->SetColor(skin->GetButtonTextColor());
    layout->AttachForm(cancelButton, nGuiFormLayout::Right, 0.005f);
    layout->AttachForm(cancelButton, nGuiFormLayout::Bottom, 0.005f);
    cancelButton->OnShow();
    this->refCancelButton = cancelButton;

    // create Ok button
    nGuiTextButton* okButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "OkButton");
    n_assert(okButton);
    okButton->SetText(this->GetText(Ok));
    okButton->SetFont("GuiSmall");
    okButton->SetAlignment(nGuiTextButton::Center);
    okButton->SetDefaultBrush("button_n");
    okButton->SetPressedBrush("button_p");
    okButton->SetHighlightBrush("button_h");
    okButton->SetMinSize(buttonSize);
    okButton->SetMaxSize(buttonSize);
    okButton->SetColor(skin->GetButtonTextColor());
    layout->AttachForm(okButton, nGuiFormLayout::Left, 0.005f);
    layout->AttachForm(okButton, nGuiFormLayout::Bottom, 0.005f);
    okButton->OnShow();
    this->refOkButton = okButton;

    // add a "delete item" buttom
    if (this->GetSaveMode())
    {
        nGuiTextButton* deleteButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "deletebutton");
        n_assert(deleteButton);
        deleteButton->SetText(this->GetText(Delete));
        deleteButton->SetFont("GuiSmall");
        deleteButton->SetAlignment(nGuiTextButton::Center);
        deleteButton->SetDefaultBrush("button_n");
        deleteButton->SetPressedBrush("button_p");
        deleteButton->SetHighlightBrush("button_h");
        deleteButton->SetMinSize(buttonSize);
        deleteButton->SetMaxSize(buttonSize);
        deleteButton->SetColor(skin->GetButtonTextColor());
        layout->AttachPos(deleteButton, nGuiFormLayout::HCenter, 0.5f);
        layout->AttachForm(deleteButton, nGuiFormLayout::Bottom, 0.005f);
        deleteButton->OnShow();
        this->refDeleteButton = deleteButton;
    }

    // optional text entry
    if (this->GetSaveMode())
    {
        nGuiTextEntry* textEntry = (nGuiTextEntry*) kernelServer->New("nguitextentry", "TextEntry");
        n_assert(textEntry);
        textEntry->SetFont("GuiSmall");
        textEntry->SetAlignment(nGuiTextEntry::Left);
        textEntry->SetDefaultBrush("textentry_n");
        textEntry->SetPressedBrush("textentry_p");
        textEntry->SetHighlightBrush("textentry_h");
        textEntry->SetCursorBrush("textcursor");
        textEntry->SetColor( this->entryTextColor );
        textEntry->SetMinSize(vector2(0.0f, textSize.y));
        textEntry->SetMaxSize(vector2(1.0f, textSize.y));
        textEntry->SetFileMode(true);
        if (!this->initialFilename.IsEmpty())
        {
            textEntry->SetText(this->initialFilename.Get());
        }
        layout->AttachForm(textEntry, nGuiFormLayout::Left, 0.005f);
        layout->AttachForm(textEntry, nGuiFormLayout::Right, 0.005f);
        layout->AttachWidget(textEntry, nGuiFormLayout::Bottom, okButton, 0.005f);
        textEntry->OnShow();
        this->refTextEntry = textEntry;
    }

    // current folder label
    nGuiTextLabel* textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "CurFolderTextLabel");
    n_assert(textLabel);
    textLabel->SetText("PATH:");
    textLabel->SetFont("GuiSmall");
    textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textLabel->SetClipping(false);
    textLabel->SetAlignment(nGuiTextLabel::Right);
    //textLabel->SetDefaultBrush("titlebar"); // layout debug
    textLabel->SetMinSize(vector2(0.0f, textSize.y));
    textLabel->SetMaxSize(vector2(1.0f, textSize.y));
    //textfield layout
    layout->AttachForm(textLabel, nGuiFormLayout::Top, 0.005f);
    layout->AttachForm(textLabel, nGuiFormLayout::Left, 0.005f);
    layout->AttachPos(textLabel, nGuiFormLayout::Right, 0.25f);
    
    textLabel->OnShow();
    this->refCurFolderLabel = textLabel;

    // current folder entry field
    nGuiTextEntry* textEntry = (nGuiTextEntry*) kernelServer->New("nguitextentry", "FolderTextEntry");
    n_assert(textEntry);
    textEntry->SetFont("GuiSmall");
    textEntry->SetAlignment(nGuiTextEntry::Left);
    textEntry->SetDefaultBrush("textentry_n");
    textEntry->SetPressedBrush("textentry_p");
    textEntry->SetHighlightBrush("textentry_h");
    textEntry->SetCursorBrush("textcursor");
    textEntry->SetColor(vector4(0.85f, 0.85f, 0.85f, 1.0f));
    textEntry->SetMinSize(vector2(0.0f, textSize.y));
    textEntry->SetMaxSize(vector2(1.0f, textSize.y));
    layout->AttachForm(textEntry, nGuiFormLayout::Top, 0.005f);
    layout->AttachWidget(textEntry, nGuiFormLayout::Left, this->refCurFolderLabel, 0.005f);
    layout->AttachForm(textEntry, nGuiFormLayout::Right, 0.005f);

    textEntry->OnShow();
    this->refFolderTextEntry = textEntry;

    // folders label
    textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "FoldersTextLabel");
    n_assert(textLabel);
    textLabel->SetText("Folders:");
    textLabel->SetFont("GuiSmall");
    textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textLabel->SetClipping(false);
    textLabel->SetAlignment(nGuiTextLabel::Left);
    //textLabel->SetDefaultBrush("titlebar"); // layout debug
    textLabel->SetMinSize(vector2(0.0f, textSize.y));
    textLabel->SetMaxSize(vector2(1.0f, textSize.y));
    //textfield layout
    layout->AttachWidget(textLabel, nGuiFormLayout::Top, this->refCurFolderLabel, 0.005f);
    layout->AttachForm(textLabel, nGuiFormLayout::Left, 0.005f);
    layout->AttachPos(textLabel, nGuiFormLayout::Right, 0.25f);
    
    textLabel->OnShow();
    this->refFolderLabel = textLabel;

    // files label
    textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "FilesTextLabel");
    n_assert(textLabel);
    textLabel->SetText("Files:");
    textLabel->SetFont("GuiSmall");
    textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textLabel->SetClipping(false);
    textLabel->SetAlignment(nGuiTextLabel::Left);
    //textLabel->SetDefaultBrush("titlebar"); // layout debug
    textLabel->SetMinSize(vector2(0.0f, textSize.y));
    textLabel->SetMaxSize(vector2(1.0f, textSize.y));
    //textfield layout
    layout->AttachWidget(textLabel, nGuiFormLayout::Top, this->refCurFolderLabel, 0.005f);
    layout->AttachWidget(textLabel, nGuiFormLayout::Left, this->refFolderLabel, 0.005f);
    layout->AttachForm(textLabel, nGuiFormLayout::Right, 0.005f);
    
    textLabel->OnShow();
    this->refFolderLabel = textLabel;

    // folder lister
    nGuiDirLister* folderLister = (nGuiDirLister*) kernelServer->New("nguidirlister", "FolderLister");
    n_assert(folderLister);
    folderLister->SetDefaultBrush("list_background");
    folderLister->SetHighlightBrush("list_selection");
    folderLister->SetFont("GuiSmall");
    folderLister->SetDirectory(this->GetDirectory());
    folderLister->SetIgnoreSubDirs(false);
    folderLister->SetIgnoreFiles(true);
    folderLister->SetSelectionEnabled(true);
    //folderLister->SetPattern(this->GetPattern());
    //folderLister->SetStripExtension(this->GetStripExtension());
    layout->AttachWidget(folderLister, nGuiFormLayout::Top, this->refFolderLabel, 0.005f);
    layout->AttachForm(folderLister, nGuiFormLayout::Left, 0.005f);
    layout->AttachPos(folderLister, nGuiFormLayout::Right, 0.25f);
    layout->AttachWidget(folderLister, nGuiFormLayout::Bottom, okButton, 0.005f);

    folderLister->OnShow();
    this->refFolderLister = folderLister;

    // file lister
    nGuiDirLister* dirLister = (nGuiDirLister*) kernelServer->New("nguidirlister", "DirLister");
    n_assert(dirLister);
    dirLister->SetDefaultBrush("list_background");
    dirLister->SetHighlightBrush("list_selection");
    dirLister->SetFont("GuiSmall");
    dirLister->SetDirectory(this->GetDirectory());
    dirLister->SetIgnoreSubDirs(true);
    dirLister->SetIgnoreFiles(false);
    dirLister->SetSelectionEnabled(true);
    dirLister->SetTextColor(this->dirlisterTextColor);
    dirLister->SetPattern(this->GetPattern());
    dirLister->SetStripExtension(this->GetStripExtension());
    layout->AttachWidget(dirLister, nGuiFormLayout::Top, this->refFolderLabel, 0.005f);
    layout->AttachWidget(dirLister, nGuiFormLayout::Left, folderLister, 0.005f);
    layout->AttachForm(dirLister, nGuiFormLayout::Right, 0.005f);

    if (this->GetSaveMode())
    {
        layout->AttachWidget(dirLister, nGuiFormLayout::Bottom, this->refTextEntry.get(), 0.005f);
    }
    else
    {
        layout->AttachWidget(dirLister, nGuiFormLayout::Bottom, okButton, 0.005f);
    }

    if ( this->initialSelection )
    {
        dirLister->SetSelectionIndex(0);
    }
    else
    {
        dirLister->SetSelectionIndex(-1);
    }

    dirLister->OnShow();

    this->refDirLister = dirLister;

    kernelServer->PopCwd();

    this->refFolderTextEntry->SetText(this->GetDirectory());


    // initialize text entry content
    if (this->GetSaveMode())
    {
        const char* curSel = this->refDirLister->GetSelection();
        if (curSel)
        {
            this->refTextEntry->SetText("");
            // FIXME: must be "curSel" instead of "" as OpenSource??
        }
    }

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiFileDialog::OnHide()
{
    if (this->refDirLister.isvalid())
    {
        this->refDirLister->Release();
        n_assert(!this->refDirLister.isvalid());
    }
    if (this->refFolderLister.isvalid())
    {
        this->refFolderLister->Release();
        n_assert(!this->refFolderLister.isvalid());
    }
    if (this->refTextEntry.isvalid())
    {
        this->refTextEntry->Release();
        n_assert(!this->refTextEntry.isvalid());
    }
    if (this->refOkButton.isvalid())
    {
        this->refOkButton->Release();
        n_assert(!this->refOkButton.isvalid());
    }
    if (this->refCancelButton.isvalid())
    {
        this->refCancelButton->Release();
        n_assert(!this->refCancelButton.isvalid());
    }
    if (this->refDeleteButton.isvalid())
    {
        this->refDeleteButton->Release();
        n_assert(!this->refDeleteButton.isvalid());
    }
    if (this->refMessageBox.isvalid())
    {
        this->refMessageBox->Release();
        n_assert(!this->refMessageBox.isvalid());
    }

    // call parent class
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGuiFileDialog::OnEvent(const nGuiEvent& event)
{
    // handle deletion warning message box events
    if (this->refDeleteMessageBox.isvalid())
    {
        if (event.GetWidget() == this->refDeleteMessageBox.get())
        {
            if (event.GetType() == nGuiEvent::DialogOk)
            {
                // delete the item
                if (this->DeleteFile())
                {
                    this->refDirLister->OnShow();
                    if (this->refDirLister->GetSelection())
                    {
                    	this->refTextEntry->SetText(this->refDirLister->GetSelection());
                }
                    else
                    {
                        this->refTextEntry->SetText("");
                    }
                }
            }
            else if (event.GetType() == nGuiEvent::DialogCancel)
            {
                // do not delete the item
            }
        }
    }
    // handle overwrite warning message box events
    if (this->refMessageBox.isvalid())
    {
        if (event.GetWidget() == this->refMessageBox.get())
        {
            if (event.GetType() == nGuiEvent::DialogOk)
            {
                // the user wants to overwrite the file
                if (this->OnOk())
                {
                    this->SetCloseRequested(true);
                }
            }
            else if (event.GetType() == nGuiEvent::DialogCancel)
            {
                // the user does not want to overwrite,
                // just return to the save game dialog
            }
        }
    }

    // if text entry exists, write current selection to text entry widget
    if (this->refTextEntry.isvalid() &&
        this->refDirLister.isvalid() &&
        (event.GetType() == nGuiEvent::SelectionChanged) &&
        (event.GetWidget() == this->refDirLister.get()) &&
        (this->refDirLister->GetSelection()))
    {
        this->refTextEntry->SetText(this->refDirLister->GetSelection());
    }

    // handle Ok, Delete and Cancel button
    if (event.GetType() == nGuiEvent::ButtonUp)
    {
        if (event.GetWidget() == this->refOkButton.get())
        {
            this->HandleOk();
        }
        else if (event.GetWidget() == this->refCancelButton.get())
        {
            if (this->OnCancel())
            {
                this->SetCloseRequested(true);
            }
        }
        else if (this->refDeleteButton.isvalid() && event.GetWidget() == this->refDeleteButton.get())
        {
            this->HandleDelete();
        }
    }
    else if (event.GetType() == nGuiEvent::SelectionDblClicked
        && event.GetWidget() == this->refDirLister.get())
    {
        // accept selection double click on DirLister as Ok
        if (!this->GetSaveMode() || !this->refMessageBox.isvalid())
        {
            // handle the save only once (the event is generated more than once, and so more than one msg are created)
            this->HandleOk();
        }
    }
    else if (event.GetType() == nGuiEvent::SelectionDblClicked
        && event.GetWidget() == this->refFolderLister.get())
    {
        const char* selDir = this->refFolderLister->GetSelection();
        if (selDir) 
        {
            nString dirName;
            if (0 == strcmp(selDir, "..")) 
            {
                // go parent
                const char * findSlash = strrchr(this->dirPath.Get(), '/'); // find last '/'
                if (findSlash) 
                {
                    nString parentPath = this->dirPath.Get();
                    *strrchr(const_cast<char *> (parentPath.Get()), '/') = 0;
                    dirName = parentPath.Get(); 
                }
                else
                {
                    // set first path
                    dirName = this->dirPath;
                }
            }
            else
            {
                // a folder
                dirName = this->refFolderLister->GetDirectory();
                dirName.Append("/");
                dirName.Append(selDir);
            }
            this->refFolderLister->SetDirectory(dirName.Get());
            this->refDirLister->SetDirectory(dirName.Get());
            this->SetDirectory(dirName.Get());
            refFolderTextEntry->SetText(dirName.Get());
        }
    }

    // call parent class
    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    Called when the Ok button is pressed. Overwrite this method in
    a subclass. Return true when the window should be closed,
    false if not.
*/
bool
nGuiFileDialog::OnOk()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Called when the Cancel button is pressed. Overwrite this method in
    a subclass. Return true when the window should be closed,
    false if not.
*/
bool
nGuiFileDialog::OnCancel()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Handle the Delete action
*/
void
nGuiFileDialog::HandleDelete()
{
    this->ExtractFilename();
    if (this->GetFilename())
    {
        // create a warning
        nGuiMessageBox* msgBox = (nGuiMessageBox*) nGuiServer::Instance()->NewWindow("nguimessagebox", false);
        msgBox->SetMessageText(this->GetText(DeleteMessage));
        msgBox->SetOkText(this->GetText(DeleteOk));
        msgBox->SetCancelText(this->GetText(DeleteCancel));
        msgBox->SetType(nGuiMessageBox::OkCancel);
        msgBox->SetTitleBar(false);
        msgBox->SetDefaultBrush("bg300x150");
        msgBox->SetModal(true);
        this->refDeleteMessageBox = msgBox;
        msgBox->Show();
    }
}

//------------------------------------------------------------------------------
/**
    Called when the user confirms deletion
*/
bool
nGuiFileDialog::DeleteFile()
{
    nString path = this->GetDirectory();
    path.Append("/");
    path.Append(this->GetFilename());

    if(nFileServer2::Instance()->DeleteFile(path.Get()))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Handle the Ok action, either the Ok button is pressed, or a double click
    on the dir lister occured.
*/
void
nGuiFileDialog::HandleOk()
{
    // extract the filename
    this->ExtractFilename();
    if (this->GetFilename())
    {
        // something valid has been selected
        // if we are in save mode and the file exists, issue a overwrite warning
        if (this->GetSaveMode())
        {
            if (!this->CheckFileExists())
            {
                // file does not exist...
                if (this->OnOk())
                {
                    this->SetCloseRequested(true);
                }
            }
            // the warning message box has been opened, this will
            // emit its own events which we need to react on
        }
        else
        {
            // this is load mode
            if (this->OnOk())
            {
                this->SetCloseRequested(true);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Extracts the filename either from the text selection or the text entry
    field.
*/
void
nGuiFileDialog::ExtractFilename()
{
    nString fname;
    if (this->refTextEntry.isvalid())
    {
        fname = this->refTextEntry->GetText();
    }
    else
    {
        fname = this->refDirLister->GetSelection();
    }
    if (!fname.IsEmpty() && !this->extension.IsEmpty())
    {
        fname.Append(".");
        fname.Append(this->extension);
    }
    this->SetFilename(fname.Get());
}

//------------------------------------------------------------------------------
/**
    Checks if the selected file exists and if yes, posts a warning message
    box. Returns true if the file exists and an overwrite warning message
    box has been created.
*/
bool
nGuiFileDialog::CheckFileExists()
{
    n_assert(this->GetSaveMode());

    nString path = this->GetDirectory();
    path.Append("/");
    path.Append(this->GetFilename());

    if (kernelServer->GetFileServer()->FileExists(path))
    {
        // file already exists, create an overwrite warning
        nGuiMessageBox* msgBox = (nGuiMessageBox*) nGuiServer::Instance()->NewWindow("nguimessagebox", false);
        msgBox->SetMessageText(this->GetText(OverwriteMessage));
        msgBox->SetOkText(this->GetText(OverwriteOk));
        msgBox->SetCancelText(this->GetText(OverwriteCancel));
        msgBox->SetType(nGuiMessageBox::OkCancel);
        msgBox->SetTitleBar(false);
        msgBox->SetModal(true);
        this->refMessageBox = msgBox;
        msgBox->Show();
        return true;
    }
    else
    {
        // file does not exist
        return false;
    }
}
