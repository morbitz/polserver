#include "StoredTokenDecoder.h"

#include "StoredToken.h"

namespace Pol::Bscript::Compiler
{
StoredTokenDecoder::StoredTokenDecoder( const std::vector<ModuleDescriptor>& module_descriptors,
                                        const std::vector<std::byte>& data )
  : module_descriptors( module_descriptors ), data( data )
{
}

void StoredTokenDecoder::decode_to( const StoredToken& tkn, fmt::Writer& w )
{
  switch ( tkn.id )
  {
  case TOK_DOUBLE:
    w << double_at( tkn.offset ) << " (float)"
      << " offset=0x" << fmt::hex( tkn.offset );
    break;

  case TOK_STRING:
  {
    auto s = string_at( tkn.offset );
    w << Clib::getencodedquotedstring( s ) << " (string)"
      << " len=" << s.length() << " offset=0x" << fmt::hex( tkn.offset );
    break;
  }

  case TOK_CONSUMER:
    w << "# (consume)";
    break;

  case CTRL_PROGEND:
    w << "progend";
    break;

  default:
    w << "id=0x" << fmt::hex( tkn.id ) << " type=" << tkn.type << " offset=" << tkn.offset
      << " module=" << tkn.module;
  }
}

double StoredTokenDecoder::double_at( unsigned offset ) const
{
  if ( offset > data.size() - sizeof( double ) )
    throw std::runtime_error( "data overflow reading double at offset " + std::to_string( offset ) +
                              ".  Data size is " + std::to_string( data.size() ) );

  return *reinterpret_cast<const double*>( &data[offset] );
}

std::string StoredTokenDecoder::string_at( unsigned offset ) const
{
  if ( offset > data.size() )
    throw std::runtime_error( "data overflow reading string starting at offset " +
                              std::to_string( offset ) + ".  Data size is " +
                              std::to_string( data.size() ) );

  const char* s_begin = reinterpret_cast<const char*>( data.data() + offset );
  const char* data_end = reinterpret_cast<const char*>( data.data() + data.size() );

  auto s_end = std::find( s_begin, data_end, '\0' );
  if ( s_end == data_end )
    throw std::runtime_error( "No null terminator for string at offset " +
                              std::to_string( offset ) );

  return std::string( s_begin, s_end );
}


}  // namespace Pol::Bscript::Compiler