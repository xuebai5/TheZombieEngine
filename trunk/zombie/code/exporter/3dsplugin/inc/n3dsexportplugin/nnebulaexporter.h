#ifndef N_NEBULAEXPORTER_H
#define N_NEBULAEXPORTER_H

#pragma warning( push, 3 )
#include "Max.h"
#pragma warning( pop )

#include "../res/n3dsexportplugin/resource.h"

#define NEBULA_EXPORTER_CLASS_ID Class_ID(0x4dc51b51, 0x150453d4)

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

class n3dsExportServer;

//------------------------------------------------------------------------------
/**
    @class nNebulaExporter
    @ingroup n3dsMaxExporterKernel
    @brief The application class of RenaissanceGame
*/
class nNebulaExporter : public SceneExport {
public:
    /* Inherit methods */
    int				ExtCount();					// Number of extensions supported
    const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
    const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
    const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
    const TCHAR *	AuthorName();				// ASCII Author name
    const TCHAR *	CopyrightMessage();			// ASCII Copyright message
    const TCHAR *	OtherMessage1();			// Other message #1
    const TCHAR *	OtherMessage2();			// Other message #2
    unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
    void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
    int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
    int	  Export(const TCHAR *name, BOOL suppressPrompts=FALSE, DWORD options=0);
    BOOL SupportsOptions(int ext, DWORD options);
    
    /*New methods*/

    nNebulaExporter();
    ~nNebulaExporter();
private:
    n3dsExportServer* exportServer;
};

#endif // __IGAMEEXPORTER__H
