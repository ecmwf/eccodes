import code_object_converter.code_objects_converter as code_objects_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.compound_statement as compound_statement
import utils.debug as debug

class CompoundStatementConverter(code_objects_converter.CodeObjectsConverter):
    
    @property
    def code_objects_class(self):
        return compound_statement.CompoundStatement
