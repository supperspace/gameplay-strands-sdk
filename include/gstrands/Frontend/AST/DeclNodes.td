include "gstrands/Frontend/AST/ASTNodes.td"

class DeclNode<DeclNode base, string diagSpelling = "", bit abstract = 0>: ASTNode {
  DeclNode Base = base;
  bit Abstract = abstract;
  string DiagSpelling = diagSpelling;
}

class DeclContext {}

def Decl: DeclNode<?, "", 1>;
def Named : DeclNode<Decl, "named declarations", 1>;
  def Namespace : DeclNode<Named, "namespaces">, DeclContext;
  def Type : DeclNode<Named, "types", 1>;
    // def Enum : DeclNode<Type, "enums">;
    def Record : DeclNode<Type, "traits, components">, DeclContext;
      def Trait: DeclNode<Record, "traits">;
      def Component: DeclNode<Record, "components">;
  def Value : DeclNode<Named, "value declarations", 1>;
    // def EnumConstant : DeclNode<Value, "enumerators">;
    def Field : DeclNode<Value, "fields">;
    def Var: DeclNode<Value, "variables">;
    def ParmVar: DeclNode<Value, "parameters">;