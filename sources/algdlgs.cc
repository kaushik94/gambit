//
// FILE: algdlgs.cc -- Classes that are often used for the algorithm
//                     parameter dialogs
//
// $Id$
//

#include "gstream.h"
#include "wxio.h"
#include "algdlgs.h"

#include "nfgconst.h"

// Need this function since the default wxGetResource takes a char **value,
// and replaces it with a newly created string (while deleting the old one).
// This is NOT what we want.

Bool wxGetResourceStr(char *section, char *entry, char *value, char *file)
{
  char *tmp_str = 0;
  Bool ok = wxGetResource(section, entry, &tmp_str, file);

  if (ok) {
    strcpy(value, tmp_str);
    delete [] tmp_str;
  }
  
  return ok;
}

//========================================================================
//            dialogAlgorithm: Member function definitions
//========================================================================

dialogAlgorithm::dialogAlgorithm(const gText &p_label, bool p_usesNfg,
				 wxWindow *p_parent, 
				 const char */*help_str*/)
  : wxDialogBox(p_parent, p_label, TRUE),
    m_usesNfg(p_usesNfg), m_depthChoice(0), m_typeChoice(0),
    m_methodChoice(0), m_markSubgames(0),
    m_stopAfter(0), m_findAll(0), m_precision(0)
{ }

dialogAlgorithm::~dialogAlgorithm(void)
{
  if (m_completed == wxOK) {
    if (m_usesNfg) {
      wxWriteResource("Soln-Defaults", "Nfg-ElimDom-Depth",
		      m_depthChoice->GetSelection(), "gambit.ini");
      if (m_depthChoice->GetSelection() != 0) {
	wxWriteResource("Soln-Defaults", "Nfg-ElimDom-Type",
			m_typeChoice->GetSelection(), "gambit.ini");
	wxWriteResource("Soln-Defaults", "Nfg-ElimDom-Method",
			m_methodChoice->GetSelection(), "gambit.ini");
      }
    }
    else {
      wxWriteResource("Soln-Defaults", "Efg-ElimDom-Depth",
		      m_depthChoice->GetSelection(), "gambit.ini");
      if (m_depthChoice->GetSelection() != 0)
	wxWriteResource("Soln-Defaults", "Efg-ElimDom-Type",
			m_typeChoice->GetSelection(), "gambit.ini");
    }

    if (m_subgames) {
      wxWriteResource("Soln-Defaults", "Efg-Mark-Subgames",
		      m_markSubgames->GetValue(), "gambit.ini");
      wxWriteResource("Soln-Defaults", "Efg-Interactive-Solns",
		      m_selectSolutions->GetValue(), "gambit.ini");
    }

    if (m_stopAfter) {
      if (m_findAll->GetValue()) 
	wxWriteResource("Algorithm Params", "StopAfter", 0, "gambit.ini");
      else
	wxWriteResource("Algorithm Params", "StopAfter",
			m_stopAfter->GetInteger(), "gambit.ini");
    }

    if (m_precision) 
      wxWriteResource("Algorithm Params", "Precision",
		      m_precision->GetSelection(), "gambit.ini");
  }
}

void dialogAlgorithm::OnOK(void)
{
  m_completed = wxOK;
  Show(FALSE);
}

void dialogAlgorithm::OnCancel(void)
{
  m_completed = wxCANCEL;
  Show(FALSE);
}

Bool dialogAlgorithm::OnClose(void)
{
  m_completed = wxCANCEL;
  Show(FALSE);
  return FALSE;
}

void dialogAlgorithm::OnDepth(void)
{
  m_typeChoice->Enable(m_depthChoice->GetSelection() > 0);
  if (m_methodChoice)
    m_methodChoice->Enable(m_depthChoice->GetSelection() > 0);
}

void dialogAlgorithm::OnAll(void)
{
  m_stopAfter->Enable(!m_findAll->GetValue());
}

void dialogAlgorithm::OnTrace(void)
{
  m_traceFile->Enable(m_traceDest->GetSelection() == 2);
  m_traceLevel->Enable(m_traceDest->GetSelection() != 0);
}

void dialogAlgorithm::DominanceFields(bool p_usesNfg)
{
  int depth = 0, type = 0, method = 0;

  if (p_usesNfg) {
    (void) new wxMessage(this, "Eliminate dominated mixed strategies");
    wxGetResource("Soln-Defaults", "Nfg-ElimDom-Depth", &depth, "gambit.ini");
    wxGetResource("Soln-Defaults", "Nfg-ElimDom-Type", &type, "gambit.ini");
    wxGetResource("Soln-Defaults", "Nfg-ElimDom-Method", &method,
		  "gambit.ini");
  }
  else {
    (void) new wxMessage(this, "Eliminate dominated behavior strategies");
    wxGetResource("Soln-Defaults", "Efg-ElimDom-Depth", &depth, "gambit.ini");
    wxGetResource("Soln-Defaults", "Efg-ElimDom-Type", &type, "gambit.ini");
  }
  NewLine();

  char *depthChoices[] = { "None", "Once", "Iterative" };
  m_depthChoice = new wxRadioBox(this, (wxFunction) CallbackDepth, "Depth",
				 -1, -1, -1, -1, 3, depthChoices);
  m_depthChoice->SetClientData((char *) this);
  if (depth >= 0 && depth <= 2) 
    m_depthChoice->SetSelection(depth);

  NewLine();

  char *typeChoices[] = { "Weak", "Strong" };
  m_typeChoice = new wxRadioBox(this, 0, "Type", -1, -1, -1, -1,
				2, typeChoices);
  if (m_depthChoice->GetSelection() == 0)
    m_typeChoice->Enable(FALSE);
  else if (type == 0 || type == 1)
    m_typeChoice->SetSelection(type);

  if (p_usesNfg) {
    char *methodChoices[] = { "Pure", "Mixed" };
    m_methodChoice = new wxRadioBox(this, 0, "Method", -1, -1, -1, -1,
				    2, methodChoices);
    if (m_depthChoice->GetSelection() == 0) 
      m_methodChoice->Enable(FALSE);
    else if (method == 0 || method == 1)
      m_methodChoice->SetSelection(method);
  }
  NewLine();
}

void dialogAlgorithm::SubgameFields(void)
{
  Bool mark = false, select = false;
  (void) new wxMessage(this, "Subgames");
  NewLine();

  wxGetResource("Soln-Defaults", "Efg-Mark-Subgames", &mark, "gambit.ini");
  wxGetResource("Soln-Defaults", "Efg-Interactive-Solns", &select,
		"gambit.ini");

  m_markSubgames = new wxCheckBox(this, 0, "Mark subgames before solving");
  m_markSubgames->SetValue(mark);
  NewLine();
  m_selectSolutions = new wxCheckBox(this, 0,
				     "Interactively select subgame solutions");
  m_selectSolutions->SetValue(select);
  NewLine();
}

void dialogAlgorithm::TraceFields(void)
{
  (void) new wxMessage(this, "Trace options");
  NewLine();
  char *traceChoices[] = { "None", "Window", "File" };
  NewLine();
  m_traceDest = new wxRadioBox(this, (wxFunction) CallbackTrace,
			       "Destination", -1, -1, -1, -1,
			       3, traceChoices);
  m_traceDest->SetClientData((char *) this);
  NewLine();
  m_traceFile = new wxText(this, 0, "File name");
  m_traceFile->Enable(FALSE);
  NewLine();
  m_traceLevel = new wxIntegerItem(this, "Level", 0, -1, -1, 100, -1);
  m_traceLevel->Enable(FALSE);
  NewLine();
}

void dialogAlgorithm::MakeCommonFields(bool p_dominance, bool p_subgames,
				       bool p_usesNfg)
{
  m_subgames = p_subgames;
  if (p_dominance)   DominanceFields(p_usesNfg);
  if (p_subgames)    SubgameFields();
  AlgorithmFields();
  TraceFields();

  wxButton *okButton = new wxButton(this, (wxFunction) CallbackOK, "Ok");
  okButton->SetClientData((char *) this);
  okButton->SetDefault();
  wxButton *cancelButton = new wxButton(this, (wxFunction) CallbackCancel,
					"Cancel");
  cancelButton->SetClientData((char *) this);
}

void dialogAlgorithm::StopAfterField(void)
{
  int stopAfter = 0;
  wxGetResource("Algorithm Params", "StopAfter", &stopAfter, "gambit.ini");
  
  m_findAll = new wxCheckBox(this, (wxFunction) CallbackAll, "Find all");
  m_findAll->SetClientData((char *) this);

  m_stopAfter = new wxIntegerItem(this, "Stop after",
				  (stopAfter > 0) ? stopAfter : 1,
				  -1, -1, 100, -1);

  if (stopAfter == 0) {
    m_findAll->SetValue(true);
    m_stopAfter->Enable(FALSE);
  }
}

void dialogAlgorithm::PrecisionField(void)
{
  int precision = 0;
  wxGetResource("Algorithm Params", "Precision", &precision, "gambit.ini");

  char *precisionChoices[] = { "Float", "Rational" };
  m_precision = new wxRadioBox(this, 0, "Precision", -1, -1, -1, -1,
			       2, precisionChoices);
  if (precision == 0 || precision == 1)
    m_precision->SetSelection(precision);;
}

gOutput *dialogAlgorithm::TraceFile(void) const
{
  switch (m_traceDest->GetSelection()) {
  case 0:
    return 0;
  case 1:
    return new gWxOutput;
  case 2:
    try {
      return new gFileOutput(m_traceFile->GetValue());
    }
    catch (gFileOutput::OpenFailed &E) {
      return 0;
    }
  default:
    return 0;
  }
}

int dialogAlgorithm::TraceLevel(void) const
{
  if (m_traceDest->GetSelection() == 0)
    return 0;
  else
    return m_traceLevel->GetInteger();
}

//=======================================================================
//                class PxiParamsDialog: Member functions
//=======================================================================

// Constructor
PxiParamsDialog::PxiParamsDialog(const char *alg, const char *label, 
                                 const char *fn, wxWindow *parent,
                                 const char *help_str)
  : dialogAlgorithm(label, false, parent, help_str)
{ }

// Make Pxi Fields
void PxiParamsDialog::MakePxiFields(void)
{
  /*
  Form()->Add(wxMakeFormNewLine());
  
  Form()->Add(wxMakeFormString("Plot Type", &type_str, wxFORM_RADIOBOX,
			       new wxList(wxMakeConstraintStrings(type_list), 0)));
  
  Form()->Add(wxMakeFormString("PxiFile", &pxiname, 
			       wxFORM_DEFAULT, NULL, NULL, 0,
			       300));
  
  Add(wxMakeFormString("Next", &name_option_str, wxFORM_RADIOBOX,
		       new wxList(wxMakeConstraintStrings(name_option_list), 0)));
  Form()->Add(wxMakeFormNewLine());
  
  Form()->Add(wxMakeFormBool("Run PXI", &run_pxi));
  
  Form()->Add(wxMakeFormString("PXI Command", &pxi_command,
			       wxFORM_DEFAULT, NULL, NULL, 0,
			       300));
  */
}

// Destructor
PxiParamsDialog::~PxiParamsDialog() { }



//=======================================================================
//                   dialogEnumPure: Member functions
//=======================================================================

#include "dlenumpure.h"

dialogEnumPure::dialogEnumPure(wxWindow *p_parent, bool p_subgames,
			       bool p_vianfg)
  : dialogAlgorithm("EnumPureSolve Parameters", p_vianfg, p_parent)
{
  MakeCommonFields(true, p_subgames, p_vianfg);
  Go();
}

dialogEnumPure::~dialogEnumPure()
{ }

void dialogEnumPure::AlgorithmFields(void)
{
  (void) new wxMessage(this, "Algorithm parameters:");
  NewLine();
  StopAfterField();
  NewLine();
}

int dialogEnumPure::StopAfter(void) const
{
  if (m_findAll->GetValue())
    return 0;
  else
    return m_stopAfter->GetInteger(); 
}

//=======================================================================
//                   dialogEnumMixed: Member functions
//=======================================================================

#include "dlenummixed.h"

dialogEnumMixed::dialogEnumMixed(wxWindow *p_parent, bool p_subgames)
  : dialogAlgorithm("EnumMixedSolve Parameters", true, p_parent,
		    ENUMMIXED_HELP)
{
  MakeCommonFields(true, p_subgames, true);
  Go();
}

dialogEnumMixed::~dialogEnumMixed()
{ }

void dialogEnumMixed::AlgorithmFields(void)
{
  (void) new wxMessage(this, "Algorithm parameters:");
  NewLine();
  StopAfterField();
  NewLine();
  PrecisionField();
  NewLine();
}

int dialogEnumMixed::StopAfter(void) const
{
  if (m_findAll->GetValue())
    return 0;
  else
    return m_stopAfter->GetInteger(); 
}

wxEnumStatus::wxEnumStatus(wxFrame *p_parent)
  : wxStatus(p_parent, "EnumMixedSolve"), pass(0)
{ }

void wxEnumStatus::SetProgress(double p_value)
{
  if (p_value > -.5)  {
    // actually, as long as its not -1.0, but floating point ==
    gauge->SetValue((int) ((p_value + pass) / 3.0 *100.0));
  }
  else {
    pass++;
  }
  wxYield();
}


//=======================================================================
//                        dialogLp: Member functions
//=======================================================================

#include "dllp.h"

dialogLp::dialogLp(wxWindow *p_parent, bool p_subgames, bool p_vianfg)
  : dialogAlgorithm("LpSolve Parameters", p_vianfg, p_parent, LP_HELP)
{
  MakeCommonFields(true, p_subgames, p_vianfg);
  Go();
}

dialogLp::~dialogLp()
{ }

void dialogLp::AlgorithmFields(void)
{
  (void) new wxMessage(this, "Algorithm Parameters:");
  NewLine();
  PrecisionField();
  NewLine();
}

//=======================================================================
//                       dialogLcp: Member functions
//=======================================================================

#include "dllcp.h"

dialogLcp::dialogLcp(wxWindow *p_parent, bool p_subgames, bool p_vianfg)
  : dialogAlgorithm("LcpSolve Parameters", p_vianfg, p_parent)
{
  MakeCommonFields(true, p_subgames, p_vianfg);
  Go();
}

dialogLcp::~dialogLcp()
{ }

void dialogLcp::AlgorithmFields(void)
{
  (void) new wxMessage(this, "Algorithm parameters");
  NewLine();
  StopAfterField();
  NewLine();
  PrecisionField();
  NewLine();
}

int dialogLcp::StopAfter(void) const
{
  if (m_findAll->GetValue())
    return 0;
  else
    return m_stopAfter->GetInteger(); 
}

//=======================================================================
//                       dialogLiap: Member functions
//=======================================================================

#include "dlliap.h"

dialogLiap::dialogLiap(wxWindow *p_parent, bool p_subgames, bool p_vianfg)
  : dialogAlgorithm("LiapSolve Parameters", p_vianfg, p_parent)
{
  MakeCommonFields(true, p_subgames, p_vianfg);
  Go();
}

dialogLiap::~dialogLiap()
{
  if (m_completed == wxOK) {
    wxWriteResource("Algorithm Params", "Liap-nTries", NumTries(), 
		    "gambit.ini");
    wxWriteResource("Algorithm Params", "Func-tolND", 
		    (float) TolND(), "gambit.ini");
    wxWriteResource("Algorithm Params", "Func-tol1D", 
		    (float) Tol1D(), "gambit.ini");
    wxWriteResource("Algorithm Params", "Func-maxitsND",
		    MaxitsND(), "gambit.ini");
    wxWriteResource("Algorithm Params", "Func-maxits1D",
		    Maxits1D(), "gambit.ini");
  }
}

void dialogLiap::AlgorithmFields(void)
{
  (void) new wxMessage(this, "Algorithm parameters:");
  NewLine();

  StopAfterField();
  NewLine();

  int nTries = 0;
  wxGetResource("Algorithm Params", "Liap-nTries", &nTries, "gambit.ini");
  m_nTries = new wxIntegerItem(this, "nTries", nTries, -1, -1, 100, -1);
  NewLine();

  float tolND = 1e-8;
  wxGetResource("Algorithm Params", "Func-tolND", &tolND, "gambit.ini");
  m_tolND = new wxNumberItem(this, "Tol n-D", tolND, -1, -1, 150, -1);
  float tol1D = 1e-8;
  wxGetResource("Algorithm Params", "Func-tol1D", &tol1D, "gambit.ini");
  m_tol1D = new wxNumberItem(this, "Tol 1-D", tol1D, -1, -1, 150, -1);
  NewLine();

  int maxitsND = 0;
  wxGetResource("Algorithm Params", "Func-maxitsND", &maxitsND, "gambit.ini");
  m_maxitsND = new wxIntegerItem(this, "Iterations n-D", maxitsND,
				 -1, -1, 150, -1);
  int maxits1D = 0;
  wxGetResource("Algorithm Params", "Func-maxits1D", &maxits1D, "gambit.ini");
  m_maxits1D = new wxIntegerItem(this, "Iterations 1-D", maxits1D,
				 -1, -1, 150, -1);
  NewLine();

  int startOption = 0;
  wxGetResource("Algorithm Params", "Start-Option", &startOption,
		"gambit.ini");
  char *startOptions[] = { "Default", "Saved", "Prompt" };
  m_startOption = new wxRadioBox(this, 0, "Start", -1, -1, -1, -1,
				 3, startOptions);
  if (startOption >= 0 && startOption <= 2)
    m_startOption->SetSelection(startOption);
  NewLine();
}

int dialogLiap::StopAfter(void) const
{
  if (m_findAll->GetValue())
    return 0;
  else
    return m_stopAfter->GetInteger(); 
}

//=======================================================================
//                    dialogSimpdiv: Member functions
//=======================================================================

#include "dlsimpdiv.h"

dialogSimpdiv::dialogSimpdiv(wxWindow *p_parent, bool p_subgames)
  : dialogAlgorithm("SimpdivSolve Parameters", true, p_parent, SIMPDIV_HELP)
{
  MakeCommonFields(true, p_subgames, true);
  Go();
}
dialogSimpdiv::~dialogSimpdiv()
{
  if (m_completed == wxOK) {
    wxWriteResource("Algorithm Params", "Simpdiv-nRestarts", NumRestarts(),
		    "gambit.ini");
    wxWriteResource("Algorithm Params", "Simpdiv-leashLength", LeashLength(),
		    "gambit.ini");
  }
}

void dialogSimpdiv::AlgorithmFields(void)
{
  (void) new wxMessage(this, "Algorithm parameters:");
  NewLine();
  StopAfterField();
  NewLine();
  PrecisionField();
  NewLine();

  int nRestarts = 0;
  wxGetResource("Algorithm Params", "Simpdiv-nRestarts", &nRestarts,
		"gambit.ini");
  m_nRestarts = new wxIntegerItem(this, "# restarts", nRestarts,
				  -1, -1, 100, -1);

  int leashLength = 0;
  wxGetResource("Algorithm Params", "Simpdiv-leashLength", &leashLength,
		"gambit.ini");
  m_leashLength = new wxIntegerItem(this, "Leash length", leashLength,
				    -1, -1, 100, -1);
  NewLine();
}  

int dialogSimpdiv::StopAfter(void) const
{
  if (m_findAll->GetValue())
    return 0;
  else
    return m_stopAfter->GetInteger(); 
}

//=======================================================================
//                    dialogPolEnum: Member functions
//=======================================================================

#include "dlpolenum.h"

dialogPolEnum::dialogPolEnum(wxWindow *p_parent,
			     bool p_subgames, bool p_vianfg)
  : dialogAlgorithm("PolEnumSolve Parameters", p_vianfg, p_parent)
{
  MakeCommonFields(true, p_subgames, p_vianfg);
  Go();
}

dialogPolEnum::~dialogPolEnum()
{ }

void dialogPolEnum::AlgorithmFields(void)
{
  (void) new wxMessage(this, "Algorithm parameters");
  NewLine();
  StopAfterField();
  NewLine();
}

int dialogPolEnum::StopAfter(void) const
{
  if (m_findAll->GetValue())
    return 0;
  else
    return m_stopAfter->GetInteger(); 
}
