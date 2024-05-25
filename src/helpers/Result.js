export default class Result {
  constructor(value, error) {
    this._value = value
    this._error = error
  }

  static ok(value) {
    return new Result(value, null)
  }

  static err(error) {
    return new Result(null, error)
  }

  isOk() {
    return this._value !== null
  }

  isErr() {
    return this._error !== null
  }

  unwrap() {
    if (this.isOk()) {
      return this._value
    } else {
      throw new Error("Tried to unwrap an error Result")
    }
  }

  unwrapErr() {
    if (this.isErr()) {
      return this._error
    } else {
      throw new Error("Tried to unwrapErr a non-error Result")
    }
  }
}
