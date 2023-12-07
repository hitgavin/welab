const Immutable = require("immutable");
const { makeEntitiesRecord, makeContentsRecord, makeStateRecord } = require("@nteract/core");
const { createContentRef, createKernelspecsRef, makeNotebookContentRecord } = require("@nteract/core");

const contentRef = createContentRef();
const kernelspecsRef = createKernelspecsRef();

const initialRefs = Immutable.Map().set(
  contentRef,
  makeNotebookContentRecord()
);

const initialState = {
  core: makeStateRecord({
    currentKernelspecsRef: kernelspecsRef,
    entities: makeEntitiesRecord({
      contents: makeContentsRecord({
        byRef: initialRefs
      })
    })
  }),
  config: Immutable.Map({
    theme: "light"
  })
};

function counterReducer(state, action) {
  const newState = { ...state };
  switch (action.type) {
    case "CREATE": {
      // we create a new object
      newState[action.payload.id] = action.payload;
      return newState;
    }
    // case "RETRIEVE": let selection = state[id];
    case "UPDATE": {
      // we do nothing if the object does not exist
      if (!Object.keys(state).includes(action.payload.id)) {
        return state;
      }
      let newObject = Object.assign(action.payload.id);
      if (action.payload.event) {
        // we update a property
        let { name, value } = action.payload.event.target;
        newObject[name] = value;
      } else {
        // we replace the object
        newObject = action.payload;
      }
      newState[action.payload.id] = newObject;
      return newState;
    }
    case "DELETE": {
      // we delete the objext
      if (!action.payload.id) {
        delete newState[action.payload];
      } else delete newState[action.payload.id];
      return newState;
    }
    default: {
      console.warn("Unhandled Action of type ", action.type);
      console.log(action.payload);
      return newState;
    }
  }
};

module.exports = {counterReducer, initialState, contentRef}