include "llvm/TableGen/SearchableTable.td"


class Severity<string N> {
  string Name = N;
}

def SEV_Ignored : Severity<"Ignored">;
def SEV_Remark  : Severity<"Remark">;
def SEV_Warning : Severity<"Warning">;
def SEV_Error   : Severity<"Error">;
def SEV_Fatal   : Severity<"Fatal">;


// Define the diagnostic classes.
class DiagClass;
def CLASS_NOTE      : DiagClass;
def CLASS_REMARK    : DiagClass;
def CLASS_WARNING   : DiagClass;
def CLASS_EXTENSION : DiagClass;
def CLASS_ERROR     : DiagClass;
def CLASS_TRAP      : DiagClass;


class Diagnostic<string summary, DiagClass DC, Severity defaultmapping> {
  string         Summary = summary;
  DiagClass      Class = DC;
  Severity       DefaultSeverity = defaultmapping;
  Diagnostic     ID = !cast<Diagnostic>(NAME);
}

class Error<string str>     : Diagnostic<str, CLASS_ERROR, SEV_Error>;
class Warning<string str>   : Diagnostic<str, CLASS_WARNING, SEV_Warning>;
class Remark<string str>    : Diagnostic<str, CLASS_REMARK, SEV_Ignored>;

// All the diagnostics the compiled can emit follow below. It might become sensible at some point to break this file down

def ParseError: Error<"parse-error">;


def DiagnosticId: GenericEnum {
    let FilterClass = "Diagnostic";
    let UnderlyingType = "uint16_t";
}

def DiagnosticClass : GenericEnum {
  let FilterClass = "DiagClass";
  let UnderlyingType = "uint8_t";
}

def DiagnosticSeverity : GenericEnum {
  let FilterClass = "Severity";
  let UnderlyingType = "uint8_t";
}

def DiagnosticsTable: GenericTable {
    let FilterClass = "Diagnostic";
    let CppTypeName = "DiagnosticInfo";

    let Fields = ["ID", "Summary", "Class", "DefaultSeverity"];

    string TypeOf_ID = "DiagnosticId";
    string TypeOf_Class = "DiagnosticClass";
    string TypeOf_DefaultSeverity = "DiagnosticSeverity";

    let PrimaryKey = ["ID"];
    let PrimaryKeyName = "lookupDiagnosticInfo";
}

