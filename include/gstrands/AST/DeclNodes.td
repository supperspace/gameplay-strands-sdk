include "gstrands/AST/ASTNodes.td"


class DeclNode<DeclNode base, string diagSpelling = "", bit abstract = 0>: ASTNode {
  DeclNode Base = base;
  bit Abstract = abstract;
  string DiagSpelling = diagSpelling;

  bit IsDeclContext = !if(!initialized(base), base.IsDeclContext, 0);
}

class DeclContext {
   bit IsDeclContext = 1;
}

def Decl: DeclNode<?, "", 1>;
def Root: DeclNode<Decl, "root declarations">, DeclContext;
def Named : DeclNode<Decl, "named declarations", 1>;
  def Namespace : DeclNode<Named, "namespaces">, DeclContext;
  // def Type : DeclNode<Named, "types", 1>;
    // def Enum : DeclNode<Type, "enums">;
  def Channel : DeclNode<Named, "channels">;
  def Record : DeclNode<Named, "traits, components", 1>, DeclContext;
    def Trait: DeclNode<Record, "traits">;
    def Component: DeclNode<Record, "components">;
  def Value : DeclNode<Named, "value declarations", 1>;
    // def EnumConstant : DeclNode<Value, "enumerators">;
    def Property : DeclNode<Value, "properties">;
    def Var: DeclNode<Value, "variables">;
    def ParmVar: DeclNode<Value, "parameters">;
def Impl: DeclNode<Decl, "implementations">;

include "gstrands/Vocab/Tokens.td"

class TerminationPolicy;

def SemicolonTerminated : TerminationPolicy;
def BraceEnclosed : TerminationPolicy;

class DeclSyntax<DeclNode node, TokenKind starter, TerminationPolicy termPolicy> {
  assert starter.IsDeclStarter, "Token is used as a declaration starter for declaration, "
                                "But it is not marked as a declaration starter in the token table";

  DeclNode Node = node;
  TokenKind Starter = starter;
  TerminationPolicy TermPolicy = termPolicy;

  // Emit the existing C++ AST enumerator, rather than generating another enum.
  string NodeKindCpp = !strconcat("Decl::", !cast<string>(node));
}

def NamespaceSyntax     : DeclSyntax<Namespace, KwNamespace, BraceEnclosed>;
def TraitSyntax         : DeclSyntax<Trait, KwTrait, BraceEnclosed>;
def ComponentSyntax     : DeclSyntax<Component, KwComponent, BraceEnclosed>;
def ChannelSyntax       : DeclSyntax<Channel, KwChannel, SemicolonTerminated>;
def ImplSyntax          : DeclSyntax<Impl, KwImpl, BraceEnclosed>;
def PropertySyntax      : DeclSyntax<Property, KwProperty, SemicolonTerminated>;

class DeclContextRule<DeclNode owner, list<DeclSyntax> children> {
  assert owner.IsDeclContext, "Owner must be a declaration context";

  DeclNode Owner = owner;
  list<DeclSyntax> AllowedDeclarations = children;
}

defvar TopLevelDeclarations = [
  NamespaceSyntax, TraitSyntax, ComponentSyntax,
  ChannelSyntax, ImplSyntax
];

def RootGrammar
  : DeclContextRule<Root, TopLevelDeclarations>;
def NamespaceGrammar
  : DeclContextRule<Namespace, TopLevelDeclarations>;
def TraitGrammar
  : DeclContextRule<Trait, [PropertySyntax]>;
def ComponentGrammar
  : DeclContextRule<Component, [PropertySyntax]>;


def DeclTerminationKind : GenericEnum {
  let FilterClass = "TerminationPolicy";
  let UnderlyingType = "uint8_t";
}

def GrammarContextKind : GenericEnum {
  let FilterClass = "DeclContextRule";
  let UnderlyingType = "uint8_t";
}

// Starter -> AST kind and termination policy.
def DeclSyntaxTable : GenericTable {
  let FilterClass = "DeclSyntax";
  let CppTypeName = "DeclSyntaxInfo";

  let Fields = ["Starter", "NodeKindCpp", "TermPolicy"];

  string TypeOf_Starter = "TokenKind";
  string TypeOf_NodeKindCpp = "code";
  string TypeOf_TermPolicy = "DeclTerminationKind";

  let PrimaryKey = ["Starter"];
  let PrimaryKeyName = "lookupDeclSyntax";
}

// Flatten each context's list into individual accepted pairs.
class DeclNestingEntry<DeclContextRule context, DeclSyntax syntax> {
  DeclContextRule Context = context;
  TokenKind Starter = syntax.Starter;
}

foreach C = !instances<DeclContextRule>() in {
  foreach S = C.AllowedDeclarations in {
    def : DeclNestingEntry<C, S>;
  }
}

// (Context, Starter) -> accepted pair; absence means disallowed.
def DeclNestingTable : GenericTable {
  let FilterClass = "DeclNestingEntry";
  let CppTypeName = "DeclNestingPolicy";

  let Fields = ["Context", "Starter"];

  string TypeOf_Context = "GrammarContextKind";
  string TypeOf_Starter = "TokenKind";

  let PrimaryKey = ["Context", "Starter"];
  let PrimaryKeyName = "lookupDeclNesting";
}