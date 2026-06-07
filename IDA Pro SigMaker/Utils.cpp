#include "Utils.h"

#ifdef _WIN32

bool SetClipboardText( std::string_view text ) {
	if( text.empty( ) ) {
		return false;
	}

	if( OpenClipboard( NULL ) == false || EmptyClipboard( ) == false ) {
		return false;
	}

	auto memoryHandle = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, text.size( ) + 1 );
	if( memoryHandle == nullptr ) {
		CloseClipboard( );
		return false;
	}

	auto textMem = reinterpret_cast<char*>( GlobalLock( memoryHandle ) );
	if( textMem == nullptr ) {
		GlobalFree( memoryHandle );
		CloseClipboard( );
		return false;
	}

	memcpy( textMem, text.data( ), text.size( ) );
	GlobalUnlock( memoryHandle );
	auto handle = SetClipboardData( CF_TEXT, memoryHandle );
	GlobalFree( memoryHandle );
	CloseClipboard( );

	if( handle == nullptr ) {
		return false;
	}

	return true;
}

#else

#include <cstdio>
#include <cstdlib>

bool SetClipboardText( std::string_view text ) {
	if( text.empty( ) ) {
		return false;
	}

	// Try xclip first, then xsel
	FILE* pipe = popen( "xclip -selection clipboard", "w" );
	if( !pipe ) {
		pipe = popen( "xsel --clipboard --input", "w" );
	}
	if( !pipe ) {
		return false;
	}

	fwrite( text.data( ), 1, text.size( ), pipe );
	pclose( pipe );
	return true;
}

#endif

bool GetRegexMatches( std::string string, std::regex regex, std::vector<std::string>& matches ) {
	std::sregex_iterator iter( string.begin( ), string.end( ), regex );
	std::sregex_iterator end;

	matches.clear( );

	size_t i = 0;
	while( iter != end ) {
		matches.push_back( iter->str( ) );
		++iter;
	}
	return !matches.empty( );
}