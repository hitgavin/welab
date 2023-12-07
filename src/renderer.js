/**
 * @file This file contains the code for rendering the React application and initializing the Jupyter kernel connection.
 */

import React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'react-redux';
import { combineReducers } from 'redux';
import { NotebookApp } from '@nteract/notebook-app-component';
import { kernels, notebooks, transforms } from '@nteract/core';
import { configureStore } from '@reduxjs/toolkit';

/**
 * Combines the reducers for kernels, notebooks, and transforms into a single root reducer.
 * @type {Function}
 */
const rootReducer = combineReducers({
  kernels,
  notebooks,
  transforms
});

/**
 * The initial state for the core reducer.
 * @type {Object}
 */
const initialCoreState = {
  kernels: kernels.makeStateRecord(),
  notebooks: notebooks.makeStateRecord()
};

/**
 * Creates the Redux store with the root reducer and initial state.
 * @type {Object}
 */

const store = configureStore({
  reducer: rootReducer,
  preloadedState: { core: initialCoreState }
});

// initialize the Jupyter kernel connection
store.dispatch(kernels.launchKernelByName({ kernelSpecName: 'python3', cwd: '.' }));

/**
 * The main React component that wraps the application with the Redux Provider.
 * @returns {JSX.Element} The rendered React component.
 */
const App = () => (
  <Provider store={store}>
    <NotebookApp />
  </Provider>
);

ReactDOM.render(<App />, document.getElementById('root'));
