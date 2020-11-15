class FileBuffer {
	buffer: ArrayBuffer
	array: Uint8Array
	offset = 0

	constructor(buffer: ArrayBuffer) {
		this.buffer = buffer
		this.array = new Uint8Array(buffer)
	}

	// Utility functions

	size() {
		return this.buffer.byteLength
	}

	ended() {
		return this.offset >= this.size()
	}

	// Reading functions

	readByte() {
		if (this.offset >= this.size()) {
			throw new Error(`Read after EOF`)
		}

		return this.array[this.offset++]
	}

	readChar() {
		const charCode = this.readByte()
		return String.fromCharCode(charCode)
	}

	readString(length: number) {
		let output = ''

		for (let i = 0; i < length; i++) {
			output += this.readChar()
		}

		return output
	}

	readUint8() {
		const asUint8 = this.readByte()

		return asUint8
	}

	readInt8() {
		const asInt8 = this.readByte()

		if (asInt8 & (1 << 7)) return ~asInt8
		return asInt8
	}

	readUint16() {
		const byte0 = this.readByte()
		const byte1 = this.readByte()
		const asUint16 = byte0 << 8 | byte1

		return asUint16
	}

	readInt16() {
		const byte0 = this.readByte()
		const byte1 = this.readByte()
		const asInt16 = byte0 << 8 | byte1

		if (asInt16 & (1 << 15)) return ~asInt16
		return asInt16
	}

	readUint32() {
		const byte0 = this.readByte()
		const byte1 = this.readByte()
		const byte2 = this.readByte()
		const byte3 = this.readByte()
		const asUint32 = byte0 << 24 | byte1 << 16 | byte2 << 8 | byte3

		// We ought do this bloody examination, for JS is utter rubbish...

		return (asUint32 < 0) ? asUint32 + 4294967296 : asUint32
	}

	readInt32() {
		const byte0 = this.readByte()
		const byte1 = this.readByte()
		const byte2 = this.readByte()
		const byte3 = this.readByte()
		const asInt32 = byte0 << 24 | byte1 << 16 | byte2 << 8 | byte3

		// What horror, JS auto-bloody-matically signs this integer!

		return asInt32
	}
}